#!/usr/bin/env ruby

require 'serialport'
require 'optparse'
require 'zlib'

CMD_EXIT              = 0x00
CMD_READ_DEVICEINFO   = 0x01
CMD_READ_DEVICESIZES  = 0x02
CMD_READ_CAPABILITIES = 0x03
CMD_READ_FLASH        = 0x10
CMD_READ_FLASH_FAST   = 0x11
CMD_WRITE_FLASH       = 0x12
CMD_WRITE_FLASH_FAST  = 0x13
CMD_ERASE_FLASH_64K   = 0x14
CMD_ERASE_FLASH_BLOCK = 0x15
CMD_CRC32_FLASH       = 0x16
CMD_READ_LOADER       = 0x20
CMD_READ_LOADINFO     = 0x21
CMD_WRITE_LOADER      = 0x22
CMD_WRITE_LOADINFO    = 0x23
CMD_LED_OFF           = 0x30
CMD_LED_ON            = 0x31
CMD_READ_DEBUGFLAGS   = 0x32
CMD_WRITE_DEBUGFLAGS  = 0x33
CMD_DIR_SETPARAMS     = 0x40
CMD_DIR_LOOKUP        = 0x41

BASIC_STARTER = [0x20, 0x60, 0xa6, 0x20, 0x8e, 0xa6, 0x4c, 0xae, 0xa7, 0x00].pack("C*").b

TIMEOUT = 2

def asc2pet(str)
  out = "".b
  str.each_char do |ch|
    chnum = ch.ord

    if chnum > 64 && chnum < 91
      chnum += 128
    elsif chnum > 96 && chnum < 123
      chnum -= 32
    elsif chnum > 192 && chnum < 219
      chnum -= 128
    elsif ch == '~'
      chnum = 255
    end

    out += chnum.chr
  end

  return out
end

def pet2asc(str)
  out = "".b
  str.each_char do |ch|
    chnum = ch.ord

    if chnum > (128+64) && chnum < (128+91)
      chnum -= 128
    elsif chnum > (96-32) && chnum < (123-32)
      chnum += 32
    elsif chnum > (192-128) && chnum < (219-128)
      chnum += 128
    elsif chnum == 255
      chnum = '~'.ord
    end

    out += chnum.chr
  end

  return out
end

def hexdump(data)
  i = 0
  while i < data.length
    printf "%04x| ",i
    16.times do |j|
      if (i+j) < data.length
        printf "%02x ", data[i+j].ord
      else
        print "   "
      end
    end

    print "|"

    16.times do |j|
      if (i+j) < data.length
        c = data[i+j].ord
        c = "."[0].ord if c < 32 || c > 0x7e
        print c.chr
      else
        print " "
      end
    end
    print "|\n"
    i += 16
  end
end

# -------

def receive(len)
  str = $port.read(len)
  str = "" if str.nil?
  return str
end

def expect(wanted)
  str = receive(wanted.length)
  if str != wanted
    puts "ERROR: Expected #{wanted}, got #{str}"
    exit 1
  end
end

def send_command(cmd) # FIXME: Add parameters here?
  $port.write(cmd.chr)
end

# -----

def read_8bit
  return receive(1).ord
end

def read_16bit
  return receive(2).unpack("v")[0]
end

def read_24bit
  return (receive(3) + 0.chr).unpack("V")[0]
end

def read_32bit
  return receive(4).unpack("V")[0]
end

def send_8bit(val)
  $port.write(val.chr)
end

def send_16bit(val)
  $port.write([val].pack("v"))
end

def send_24bit(val)
  str = [val].pack("V")
  $port.write(str[0,3])
end

# -----

def led_on
  send_command(CMD_LED_ON)
  expect("OK")
end

def led_off
  send_command(CMD_LED_OFF)
  expect("OK")
end

def read_deviceinfo
  send_command(CMD_READ_DEVICEINFO)

  str = ""
  loop do
    ch = receive(1)
    break if ch.ord == 0
    str += ch.chr
  end

  expect("OK")
  
  return str
end

def read_devicesizes
  send_command(CMD_READ_DEVICESIZES)

  totalsize = read_24bit
  pagesize  = read_16bit
  erasesize = read_16bit

  expect("OK")
  return [totalsize, pagesize, erasesize]
end

def write_loader(data)
  send_command(CMD_WRITE_LOADER)

  $stderr.puts "ERROR: Loader length #{data.length} is incorrect!" unless data.length == 171

  $port.write(data)
  expect("OK")
end

def read_loader
  send_command(CMD_READ_LOADER)

  data = receive(171)

  expect("OK")
  return data
end

def erase_page(addr)
  send_command(CMD_ERASE_FLASH_BLOCK)
  send_24bit(addr)
  expect("OK")
end

def write_flash(addr, len, data)
  if len > 65535
    $stderr.puts "ERROR in write_flash: invalid length #{len} specified"
    exit 99
  end

  send_command(CMD_WRITE_FLASH)
  send_24bit(addr)
  send_16bit(len)
  $port.write(data)
  expect("OK")
end

def crc32_flash(addr, len)
  send_command(CMD_CRC32_FLASH)
  send_24bit(addr)
  send_24bit(len)

  crc32 = read_32bit
  expect("OK")
  return crc32
end

def read_loadinfo
  send_command(CMD_READ_LOADINFO)
  dataaddr = read_16bit
  datalen  = read_16bit
  calladdr = read_16bit
  fname    = receive(16)

  expect("OK")
  return [dataaddr, datalen, calladdr, fname]
end

def write_loadinfo(dataaddr, datalen, calladdr, fname)
  send_command(CMD_WRITE_LOADINFO)
  send_16bit(dataaddr)
  send_16bit(datalen)
  send_16bit(calladdr)

  while fname.length < 16
    fname += " "
  end

  $port.write(fname[0,16])

  expect("OK")
end

# ------

loader_file = nil
data_file   = nil
data_length = nil
data_name   = nil
data_addr   = nil
call_addr   = nil

# parse command line args
OptionParser.new do |opts|
  opts.on_tail("-h", "--help", "Show this message") do
    puts opts
    exit 1
  end

  opts.on("-l", "--loader FILE", "write a loader to the tapecart") do |arg|
    loader_file = arg
  end

  opts.on("-n", "--name STRING", "set displayed file name") do |arg|
    data_name = asc2pet(arg)
  end

  opts.on("-c", "--calladdr ADDRESS", "set call address") do |arg|
    call_addr = arg.to_i(0)
  end

  opts.on("-L", "--length NUM", "set initial data length") do |arg|
    data_length = arg.to_i(0)
  end

  opts.on("-o", "--offset NUM", "set initial data offset") do |arg|
    data_addr = arg.to_i(0)
    if data_addr > 0xffff
      puts "ERROR: Data offset must be below 0x10000"
      exit 2
    end
  end
end.parse!

if ARGV.length > 1
  puts "ERROR: Too many files specified"
  exit 2
end

data_file = ARGV[0] if ARGV.length == 1
data_name = asc2pet(data_file) if data_name.nil? && !data_file.nil?

$port = SerialPort.new("/dev/ttyAMA0").binmode
$port.flow_control = SerialPort::NONE
$port.baud         = 115200

# discard existing data
$port.read_timeout = 100
while !$port.getc.nil? do
end
$port.read_timeout = TIMEOUT * 1000

# turn on LED while active
led_on

# read device data
puts pet2asc(read_deviceinfo)

(totalsize, pagesize, erasesize) = read_devicesizes

puts "Total size: #{totalsize} bytes"
puts "Page size : #{pagesize} bytes"
puts "Erase size: #{erasesize * pagesize} bytes"


if !loader_file.nil?
  # write a loader
  loader = File.binread(loader_file)

  while loader.length < 171
    loader += 0x00.chr.b
  end

  puts "writing loader"
  write_loader(loader)

  # verify it
  puts "reading loader"
  readback = read_loader

  if loader != readback
    puts "Loader verify mismatch: got #{readback.length} byte"
    exit 1
  end
end

# write datafile
if !data_file.nil?
  data = File.binread(data_file)

  # build onefiler-data if needed
  is_onefiler = false

  if data_length.nil?
    data_length = data.length
    is_onefiler = true
  end

  if call_addr.nil?
    call_addr = data.unpack("v")[0]
  end

  if is_onefiler && call_addr == 0x0801
    # prepend BASIC starter
    call_addr = 0x0801 - BASIC_STARTER.length
    data = [call_addr].pack("v") + BASIC_STARTER + data[2..-1]
    data_length = data.length
  end

  # pad to a multiple of the erase size
  if (data.length % (erasesize * pagesize)) != 0
    remain = erasesize * pagesize - data.length % (erasesize * pagesize)
    data += 0xff.chr * remain
  end

  puts "Writing #{data.length} bytes"

  erase_countdown = 0
  addr = 0 # FIXME: Add an option to specify a different start address
  data_addr = 0
  (data.length / pagesize).times do |page|
    print "Writing page #{page}\r"
    $stdout.flush
    if erase_countdown == 0
      erase_page(page * pagesize)
      erase_countdown = erasesize
    end

    write_flash(addr, pagesize, data[addr, pagesize]) # FIXME: assumes addr starts at 0
    addr += pagesize
    erase_countdown -= 1
  end

  # verify using CRC32
  # (in chunks to avoid serial RX timeouts)
  (data.length / pagesize).times do |page|
    print "Verifying page #{page}\r"
    tc_crc = crc32_flash(page * pagesize, pagesize)
    my_crc = Zlib.crc32(data[page * pagesize, pagesize])
    
    if tc_crc != my_crc
      printf "ERROR: CRC32 validation failed, tc got 0x%08x, script got 0x%08x in page %d", tc_crc, my_crc, page
    end
  end
  puts "\nCRC32-verify ok"
end

# write loadinfo
if !call_addr.nil? || !data_name.nil?
  (tc_addr, tc_len, tc_calladdr, tc_name) = read_loadinfo
  tc_addr     = data_addr   unless data_addr.nil?
  tc_len      = data_length unless data.nil?
  tc_calladdr = call_addr   unless call_addr.nil?
  tc_name     = data_name   unless data_name.nil?
  write_loadinfo(tc_addr, tc_len, tc_calladdr, tc_name)

  puts "addr 0x#{sprintf("%04x", tc_addr)} len 0x#{sprintf("%04x", tc_len)} ",
       "calladdr 0x#{sprintf("%04x", tc_calladdr)} name #{pet2asc(tc_name)}"
end

# done, turn off LED
led_off
$port.close
