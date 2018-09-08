require "styx"

print (styx)
for k,v in pairs(styx) do
    print(k, v)
end

styx.add_file("testtest")

styx.serve {}
