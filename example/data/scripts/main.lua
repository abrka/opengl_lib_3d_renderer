local LuaScript1 = {
	i = 0
}

function LuaScript1:tick()
	if LuaScript1.i < 10 then 
		print(LuaScript1.i)
		LuaScript1.i = LuaScript1.i + 1 
	end
end

return LuaScript1
