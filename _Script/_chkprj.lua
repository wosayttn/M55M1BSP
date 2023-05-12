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


print("This code is used to check the target device setting in KEIL and IAR")

local path = ""
local file
local data = ""

-- Check device setting in KEIL project

file = io.popen('dir ..\\*.uvproj /b /s')
data = file:read("*a")
file:close()

-- Search KEIL project files
p = {}
i = 0
for path in string.gmatch(data, "(.-)\n") do
	if path ~= nil then
	    idx = string.find(path, "Library")
	    if(idx ~= nil)then
            print("Skip "..path)
	    else
            p[i] = path
    	    i = i+1;
	    end
	end
end

-- Read each file
for idx,path in pairs(p) do

    -- read file
    file = io.open(path, "rt")
    data = file:read("*all")
    file:close()

    -- find device
    for dev in string.gmatch(data, "<Device>(.-)</Device>") do
        if(dev ~= nil)then
            if(dev ~= "NUC121SC2AE")then
                print("\nERROR:")
                print(path)
                print("Device setting is ["..dev.."]")
            end
        end
    end

    i = 1
    local gn = {}
    gn[1] = "CMSIS"
    gn[2] = "User"
    gn[3] = "Library"
    for gname in string.gmatch(data, "<GroupName>(.-)</GroupName>") do
        if(gname ~= gn[i])then
            print("\nERROR:")
            print(path)
            print("Group name: ["..gname.."] Should be ["..gn[i].."]")
        end
        i = i + 1
        if(i >= 4)then
            i = i - 3
        end
    end


end

-- Check device setting in IAR project

file = io.popen('dir ..\\*.ewp /b /s')
data = file:read("*a")
file:close()

-- Search IAR project files
p = {}
i = 0
for path in string.gmatch(data, "(.-)\n") do
	if path ~= nil then
	    idx = string.find(path, "Library")
	    if(idx ~= nil)then
            print("Skip "..path)
	    else
            p[i] = path
    	    i = i+1;
	    end
	end
end

-- Read each file
for idx,path in pairs(p) do
    local ovflag = 0

    -- read file
    file = io.open(path, "rt")
    data = file:read("*all")
    file:close()

    -- find device
    for dev in string.gmatch(data, "<name>OGChipSelectEditMenu</name>(.-)</state>") do
        if(dev ~= nil)then
            dev = trim(dev)
            if(dev ~= "<state>NUC121AE series	Nuvoton NUC121AE series")then
                print("\nERROR:")
                print(path)
                print("Device setting is ["..dev.."]")
            end
        end
    end


    -- find download settings
    local path_ewd = ""
    local setting = ""
    path_ewd = string.gsub(path, ".ewp", ".ewd")
    file = io.open(path_ewd, "rt")
    data = file:read("*all")
    file:close()

    -- check download verify setting
    for setting in string.gmatch(data, "<name>OCDownloadVerifyAll</name>(.-)</state>") do
        setting = trim(setting)
        if(setting ~= "<state>1")then
            print("\nERROR:")
            print(path_ewd)
            print("OCDownloadVerifyAll setting is wrong. It must be 1")
        end
    end

    -- check debug driver setting
    for setting in string.gmatch(data, "<name>OCDynDriverList</name>(.-)</state>") do
        setting = trim(setting)
        if(setting ~= "<state>THIRDPARTY_ID")then
            print("\nERROR:")
            print(path_ewd)
            print("OCDynDriverList setting is wrong. It must be THIRDPARTY_ID")
        end
    end


    -- check flash loader setting
    for setting in string.gmatch(data, "<name>UseFlashLoader</name>(.-)</state>") do
        setting = trim(setting)
        if(setting ~= "<state>1")then
            print("\nERROR:")
            print(path_ewd)
            print("UseFlashLoader setting is wrong. It must be 1")
        end
    end

    -- check over ride setting
    ovflag = 0
    for setting in string.gmatch(data, "<name>OverrideDefFlashBoard</name>(.-)</state>") do
        setting = trim(setting)
        if(setting ~= "<state>0")then
            print("\nWARN:")
            print(path_ewd)
            print("OverrideDefFlashBoard enabled. Please check boad setting.")
            ovflag = 1
        end
    end

    if (ovflag == 1) then
        for setting in string.gmatch(data, "<name>FlashLoadersV3</name>(.-)</state>") do
            setting = trim(setting)
            setting = string.match(setting, "<state>(.+)")
            print(setting)
        end
    end
end
