script="dweet-af104-test.lua"; version="16.2.14" 
print(script..", version "..version)


ledESP=4 -- blue LED on ESP-12 
frequency=500 -- maximum is 1000
dutycycle=1000 -- 0..1023 (maximun is HIGH=off)
pwm.setup(ledESP, frequency, dutycycle)
pwm.start(ledESP)

dofile('sntp.lua')
tmr.delay(1000)

_af104_status="n/a"

pl = nil;

     -- create a server  
sv=net.createServer(net.TCP, 10) 
    -- server listen on port 80
    sv:listen(8080,function(conn)
		conn:on("receive", function(conn, pl) 
		payload = pl;
        print(payload.."\n")
        -- see ISSUE below -- MG
--		conn:send("<html />")
--		conn:send("<body bgcolor='#E6E6E6'>")
--		conn:send("<B/>ESP8266 chipID: "..node.chipid())
--		conn:send("<br/>Running....!")
    _UTC=rtctime.get()                     -- sensor3
    h, m, s, Y, M, D, W = unix2time(_UTC)
    if (h+1) == 24 then h=0 else h=h+1 end -- avoid 24:01:01 
    _GMT=string.format("%02d:%02d:%02d", h, m, s)

        local sendTextHTML = "<html /><body bgcolor='#FFFFFF'><B/>NodeMCU script: "..
              script..", version: "..version.."<br/>"..
              _GMT.."<br/>"..
              _af104_status.."<br/>Running....!\n"
        -- conn:send(sendTextHTML) -- MG
------------------------------------------------------------------------        
		if string.sub(pl, 0, 11) == "**command**"  then
             dofile("wifi_tools.lua")
        end 
-------------------------------------------------------------------------         
        if string.find(pl, "GET / HTTP") then
            dofile("getstatus.lua")
            tmr.delay(250)
            file.open("info.lua", "r")
--          conn:send("\n")
--          conn:send(file.read())
            sendTextHTML = sendTextHTML.."\n"..file.read()
            conn:send(sendTextHTML) -- MG
            file.close("info.lua")
         end
        if string.find(pl, "/status") then
            dofile("getstatus.lua")
            tmr.delay(250)
            file.open("info.lua", "r")
            sendTextHTML = sendTextHTML.."\n"..file.read()
            conn:send(sendTextHTML) -- MG
            file.close("info.lua")
         end
        if string.find(pl, "/af104/in") then
            _af104_status="In"
            sendTextHTML = "Noted: IN \n"
            conn:send(sendTextHTML) 
         end
        if string.find(pl, "/af104/out") then
            _af104_status="Out"
            sendTextHTML = "Noted: OUT \n"
            conn:send(sendTextHTML) 
         end
		conn:close()
		collectgarbage()
		end)
end)
print("Server running...")

-- ISSUES:
-- Ouch, this is broke in 1.4 and 1.5. Until I can rewrite the code, it will only work with older versions.
-- This issue is documented at:
-- https://github.com/nodemcu/nodemcu-firmware/issues...
--
-- Ok I made it work. 
-- Essentially I cut to code to send just one conn:send command that includes everything. 
-- It appears to send only the first conn-send command.
-- So for servernode.lua, i commented lines: 8-11, 22 and voila.




thing="af104-test"
-- sensors names should not contain dots to displa on dweet.io Discover-page
scriptV="script_Version"
sensor1="elapsed_Time"
sensor2="rssi_Station"
sensor3="unix_Epoch"
sensor4="date_Time"
listapRssi="-99"

command="GET /dweet/for/"
protocol="HTTP/1.1\r\nHost: dweet.io\r\n".."Connection: close\r\nAccept: */*\r\n\r\n"

-- get RSSI from current STATION
function listap(t)
    for bssid,v in pairs(t) do
        local ssid, rssi, authmode, channel = string.match(v, "([^,]+),([^,]+),([^,]+),([^,]*)")
        print("CURRENT RSSI IS: "..rssi)
        listapRssi=rssi
    end
    return listapRssi
end
ssid, pwd, bssid_set, bssid=wifi.sta.getconfig() 
scan_cfg = {}
scan_cfg.ssid = ssid 
if bssid_set == 1 then scan_cfg.bssid = bssid else scan_cfg.bssid = nil end
scan_cfg.channel = wifi.getchannel()
scan_cfg.show_hidden = 0
ssid, pwd, bssid_set, bssid=nil, nil, nil, nil
wifi.sta.getap(scan_cfg, 1, listap)
print("RSSI testing: "..listapRssi.." dBm")

-- convert unix epoch time to date&time
-- return h, m, s, Y, M, D, W (1-mon, 7-sun)
function unix2time(t)
    local jd, f, e, h, y, m, d
    jd = t / 86400 + 2440588
    f = jd + 1401 + (((4 * jd + 274277) / 146097) * 3) / 4 - 38
    e = 4 * f + 3
    h = 5 * ((e % 1461) / 4) + 2
    d = (h % 153) / 5 + 1
    m = (h / 153 + 2) % 12 + 1
    y = e / 1461 - 4716 + (14 - m) / 12
    return t%86400/3600, t%3600/60, t%60, y, m, d, jd%7+1
end

tmr.alarm(0, 10000, 1, function() 
    elapsedTime = tmr.time()              -- sensor1
    wifi.sta.getap(scan_cfg, 1, listap)   -- sensor2 (updates listapRssi)
    UTC=rtctime.get()                     -- sensor3
    h, m, s, Y, M, D, W = unix2time(UTC)
    if (h+1) == 24 then h=0 else h=h+1 end -- avoid 24:01:01 
    GMT=string.format("%02d:%02d:%02d", h, m, s)
    D_T=GMT                               -- sensor4 
    stringPOST = command..thing.."?"..
                 sensor1.."="..elapsedTime..
                 "&"..
                 sensor2.."="..listapRssi..
                 "&"..
                 sensor3.."="..UTC..
                 "&"..
                 sensor4.."="..D_T..
                 "&"..
                 scriptV.."="..version..                 "&"..

                 " ".. protocol
    sk=net.createConnection(net.TCP,0) 
    sk:on("receive", function(sck, pl) print("response: ",pl)
        pwm.setup(ledESP, frequency, 1000); pwm.start(ledESP)
        --print(tmr.now().." receive: duty cycle = "..pwm.getduty(ledESP))
    end)
    sk:on("connection",function(sck, payload)
        pwm.setup(ledESP, frequency, 255); pwm.start(ledESP)
        --print(tmr.now().." connection: duty cycle = "..pwm.getduty(ledESP))
        sk:send(stringPOST)
    end)
    sk:connect(80,"dweet.io")
end )


