local M = {
	i = 0
}

function M:init()
	print("I was init")
end


function M:tick()
	if self.i < 10 then 
		print("my id is:" , self.entity_id , "iteration is:" , self.i)
		self.i = self.i + 1 
	end
end

return M
