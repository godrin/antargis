class Level1<AntLevelScript
	def eventLevelStarted
		if not @started
			start=StoryFlow.new("beginning")

			start.push("Tutorial - Constructing","On this island you will learn about constructing.")
			start.push("Tutorial - Constructing","You have to conquer the tower in the north. But it is too strong for you.")
			start.push("Tutorial - Constructing","So you must increase your strength. To do so take over the workshop and press the construct-button when you have selected the workshop.")
			tellStory(start)
			@started=true
		end
	end
	def eventHLJobFinished(hero,job)
		if hero.getName!="Rowen"
			return
		end
		if job.is_a?(AntHeroFightJob)
			if job.target.is_a?(AntWorkshop)
				@defeat=true
				start=StoryFlow.new("sheep")
				start.push("Tutorial","Press the construct button. But beware that your men do rest enough.")
				start.push("Tutorial","When you have enough weopons and armour you can take it all up by pressing the 'take weapon' button.")
				tellStory(start)
			else
				if hero.getName=="Rowen"
					wonLevel
				else
					lostLevel
				end
			end
		end
	end
	def eventTrigger(hero,t)
		case t.name
			when "storyFinished"
				if @story.name=="end"
					endLevel
				end
		end
		Log.debug "TRIGGER"
		return false #ignore
	end
	def wonLevel
		return if @won 
		@interface.wonLevel
		#super
		@won=true
		start=StoryFlow.new("end")
		start.push("Tutorial","You finished this level.")
		tellStory(start)
	end
	def lostLevel
		return if @lost 
		@interface.lostLevel
		#super
		@lost=true
		start=StoryFlow.new("end")
		start.push("Tutorial","You lost!")
		tellStory(start)
	end
	def eventHeroDied(ent)
		case ent.getName	
			when "Rowen"
				lostLevel
		end
	end

	def tellStory(story)
		@story=story
		@interface.tellStory(story)
	end
	def endLevel
		@interface.endLevel
	end

end