-- NodeMCU /docs example
-- requires master custom build with modules: sntp, rtctime

-- Best effort, use the last known NTP server (or the NTP "anycast" address 224.0.1.1 initially) -- 
-- sntp.sync() 

-- modified: Michael Gries 2016-02-01 - function unix2date from BME280 project included

UTC=1454000000
GMT="2016-01-01T00:00:00Z GMT"

-- convert unix epoch time to date&time
-- return h, m, s, Y, M, D, W (1-mon, 7-sun)
local function unix2date(t)
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

-- Sync time with 192.168.0.1 and print the result, or that it failed 
sntp.sync('pool.ntp.org', 
	function(sec,usec,server) 
		print('sync', sec, usec, server) 
        UTC=rtctime.get()
        print('rtctime.get(): '..UTC.." sec (Unix Epoch)")
        h, m, s, Y, M, D, W = unix2date(UTC)
        GMT=string.format("%d-%02d-%02dT%02d:%02d:%02dZ UTC", Y, M, D, h, m, s)
        print("Date&Time: "..GMT)
	end, 
	function() 
		print('sntp sync failed!') 
	end
) 

-- result on 2016-01-26
-- dofile("sntp.lua");
-- sync    1453846643  223150  85.214.193.52
-- rtctime.get(): 1453847039 sec (Unix Epoche)

