-- init.lua
-- (c) Michael Gries, created: 2016-01-17, modified: 2016-01-23

-- script = 'dweet-af104-test.lua'
script = 'servernode.lua'

function main()
    dofile(script)
    end

print("run script '" .. script .. "' after 15s - sntp sync")
tmr.alarm(0,15000,0,main)
