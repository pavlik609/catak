attacks = {{}}
require "gaster_blaster"
attack_lib = {["gb"] = GasterBLAST}

function parse_attack(filename)
    if Misc.FileExists(filename) == true then
        file = Misc.OpenFile( filename,"r") 
        lines = file.ReadLines()
        for i=1,#lines do
            local split = {}
            for j in string.gmatch(lines[i], "%S+") do
                split[#split+1] = j
            end
            if attacks[tonumber(split[1])] == nil then
                attacks[tonumber(split[1])] = {}
            end
            attacks[tonumber(split[1])][i] = {}
            attacks[tonumber(split[1])][i][1] = attack_lib[split[2]]
            attacks[tonumber(split[1])][i][2] = {["startX"]=tonumber(split[3]),["startY"]=tonumber(split[4]),
                                              ["endX"]  =tonumber(split[5]),["endY"]  =tonumber(split[6]),
                                            ["rotation"]=tonumber(split[7]),["rotationEnd"]=tonumber(split[8])}
        end
    end
    SetGlobal("catak_attacks",attacks)
end