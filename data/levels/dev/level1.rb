class Level1<AntLevelScript
	def eventLevelStarted
		Log.debug "Level inited"
	end
	def eventTrigger(hero,t)
    Log.debug "TRIGGER"
		return false #ignore
	end
	def eventOwnerChanged(ent)
		case ent.getName	
			when "Rowen"
				lostLevel
			when "Bantor"
				wonLevel
		end
	end

end