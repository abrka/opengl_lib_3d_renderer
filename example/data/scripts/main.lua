local M = {
	i = 0
}

function M:init()
	print("[SCRIPT] I was init")
	local name_comp = self.components["NameComponent"]
	print("[SCRIPT] My name is", name_comp.name)
	name_comp.name = "new name set from lua"
	print("[SCRIPT] My new name is", name_comp.name)
end


function M:tick()
	if self.i < 10 then 
		print("[SCRIPT] my id is:" , self.entity_id , "iteration is:" , self.i)
		self.i = self.i + 1 
	end
end

return M
