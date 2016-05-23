-- Unix like commands module
-- LICENCE: http://opensource.org/licences/MIT
-- 2015.07.09 WG, 2016-01-28 MG
--
-- Commands: ls(), cat(filename). iwlist()
--
-- Example:
-- dofile("unix.lua").ls()   
-- dofile("unix.lua").cat("unix.lua")
-- dofile("unix.lua").iwlist()
local M
do
-- cat -shows file content
local cat = function(filename)
    file.open(filename,"r")
    local l = file.readline()
    while l ~= nil do
	print(l:match("^(.-)%s*$"))
	l = file.readline()
    end
    file.close()
end
-- ls - list all filenames
local function ls()
    print("Memory (heap):", node.heap())
    print("Disk (flash: Remain Used Total):", file.fsinfo())
    local l = file.list();
    for k,v in pairs(l) do
	print(k, v)
    end
    l = nil
end
-- iwlist - list all Wifi Access Points
local function iwlist()
    print("AP list:")
    wifi.sta.getap(function(t)
        for k,v in pairs(t) do 
	    print(k,v)
	end
    end)
end
-- export functions
    M = { cat = cat, ls = ls, iwlist = iwlist }
end
return M
