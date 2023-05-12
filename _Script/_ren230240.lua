replist = {}

-- keyword rename mapping table
replist[1] = {[1] = "NUC200", [2] = "NUC230_240"}
replist[2] = {[1] = "NUC200Series", [2] = "NUC230_240"}


function trim(str)

    if(str == nil) then
        return ""
    end

    -- Remove space
    str = str:gsub("^%s*(.-)%s*$", "%1")

    -- Remove new line
    str = str:gsub("\n","")

    return str
end


function repword(instr)
    
    local n
    local i
    local msg = ""

    instr = trim(instr)
    
    
    for i, item in pairs(replist) do
        if(instr == item[1])then
            instr = string.gsub(instr, item[1], item[2])
            msg = string.format("%-30s ==> %-30s",item[1], item[2])
            print(msg) 
        end
    end
    
    return instr 

end

function split(pString, pPattern)
   local Table = {}  -- NOTE: use {n = 0} in Lua-5.0
   local fpat = "(.-)" .. pPattern
   local last_end = 1
   local s, e, cap = pString:find(fpat, 1)
   while s do
      if s ~= 1 or cap ~= "" then
     table.insert(Table,cap)
      end
      last_end = e+1
      s, e, cap = pString:find(fpat, last_end)
   end
   if last_end <= #pString then
      cap = pString:sub(last_end)
      table.insert(Table, cap)
   end
   return Table
end

function IsValidPath(path)
local list = {}

-- Skip path list
list[1] = "Library\\CMSIS"
list[2] = "_scripts\\MiniDoxyfile"

-- local variables
local i;
local str = ""
local idx = 0
    
-- Search keyword with list
    for i,str in pairs(list) do
        idx = string.find(path, str)
        if(idx ~= nil)then
            break
        end
    end

-- return true if no keyword in list found    
    if(idx ~= nil)then
        return false
    else
        return true
    end
end

function FindFiles(fstr, p, i)
    
    local file
    local data = ""
    local tmpstr = ""
    
    tmpstr = 'dir .\\'..fstr..' /b /s'..' 2> nil'
    file = io.popen(tmpstr)
    data = file:read("*a")
    file:close()
    
    for path in string.gmatch(data, "(.-)\n") do
    	if path ~= nil then
            if(IsValidPath(path))then	
                p[i] = path
        	    i = i+1;
    	    end
    	end
    end
    
    return i
end

function ReplaceStrInFile(filename)
    
    local file
    local data = ""
    local fname = ""
    local tmpstr = ""
    
    file = io.open(filename, "r+t")
    if(file ~= nil)then
        data = file:read("*all")
        file:close()
    end
    
    -- replace all string
    data = string.gsub(data, "(%w+)", repword)
    
    
    -- new file name    
    -- fname = string.gsub(filename, "(%..-)$","%.txt")
    
    -- Update file
    fname = filename
    
    file = io.open(fname,"w+t")
    if(file ~= nil)then
        file:write(data)
        file:close()
    end
end

function UpdateKeilProj(filename)

    local file
    local data = ""
    
    -- read project file
    file = io.open(filename, "r+t")
    if(file ~= nil)then
        data = file:read("*all")
        file:close()
    end

    -- replace - system_NUC200Series.s ==> system_NUC230_240.s

    data = string.gsub(data, "startup_NUC200Series.s", "startup_NUC230_240.s")
    data = string.gsub(data, "system_NUC200Series.s", "system_NUC230_240.s")
    data = string.gsub(data, "\\NUC200Series\\", "\\NUC230_240\\")
    data = string.gsub(data, "startup_NUC200Series.o%(RESET%)", "startup_NUC230_240.o%(RESET%)")
    data = string.gsub(data, "system_NUC200Series.c", "system_NUC230_240.c")
    
    file = io.open(filename,"w+t")
    if(file ~= nil)then
        file:write(data)
        file:close()
    end
end


local file
local data = ""
local p = {}    -- For the file list
local i = 1
local j
local cmd = ""

-- Search all .c files
i = FindFiles('..\\*.c', p, i)
-- Search all .h files
i = FindFiles('..\\*.h', p, i)
-- Search all .s files
i = FindFiles('..\\*.s', p, i)
-- Search all .txt files
i = FindFiles('..\\*.txt', p, i)

-- total number of files to be renamed
total = i

for i,path in pairs(p) do

    -- Get the filename
    --spath = split(path, "\\")
    --j = table.getn(spath)
    --print(spath[j])
    --fname = string.match(spath[j], "(%..-)$")
    --print(fname)
    data = string.format("[ %-95s ]", path)
    print(data)    
    ReplaceStrInFile(path)

end

-- Update KEIL project files
-- Search all .uvproj, .ewp files
i = FindFiles('..\\*.uvproj', p, i)
i = FindFiles('..\\*.ewp', p, i)
for i,path in pairs(p) do
    
    data = string.format("[ %-95s ]", path)
    print(data)    
    UpdateKeilProj(path)

end




-- rename device file name
cmd = "ren ..\\Library\\Device\\Nuvoton\\NUC200Series\\Include\\NUC200Series.h NUC230_240.h"
print(cmd)
os.execute(cmd)

cmd = "ren ..\\Library\\Device\\Nuvoton\\NUC200Series\\Include\\system_NUC200Series.h system_NUC230_240.h"
print(cmd)
os.execute(cmd)

cmd = "ren ..\\Library\\Device\\Nuvoton\\NUC200Series\\Source\\system_NUC200Series.c system_NUC230_240.c"
print(cmd)
os.execute(cmd)

cmd = "ren ..\\Library\\Device\\Nuvoton\\NUC200Series\\Source\\ARM\\startup_NUC200Series.s startup_NUC230_240.s"
print(cmd)
os.execute(cmd)

cmd = "ren ..\\Library\\Device\\Nuvoton\\NUC200Series\\Source\\IAR\\startup_NUC200Series.s startup_NUC230_240.s"
print(cmd)
os.execute(cmd)


-- Rename the M051 in path
cmd = "ren ..\\Library\\Device\\Nuvoton\\NUC200Series NUC230_240"
print(cmd)
os.execute(cmd)






