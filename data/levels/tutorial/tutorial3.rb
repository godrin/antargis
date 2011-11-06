class Level1<AntLevelScript
	def eventLevelStarted
		if not @started
			start=StoryFlow.new("beginning")
			t="Tutorial - Building"

			start.push(t,"You are stranded on this small island. In the north there is the continent that you want to reach.")
			start.push(t,"It's a pity that you don't have any boats to get there. You don't have much food, either.")
			start.push(t,"Build a tower. After that dismiss some of your troops so they can gather some food.")
			start.push(t,"When you are finished with this you can continue to build some food production cycle with farm, mill and bakery. At last you'll be able to build a workshop and get some boats.")
			start.push(t,"But ... \nchoose the positions of the buildings well!")
			tellStory(start)
			@started=true
		end
	end

	def eventTrigger(hero,t)
		case t.name
			when "storyFinished"
				if @story.name=="end"
					endLevel
				end
			when "continent"
				start=StoryFlow.new("end")
				t="Tutorial - Building"
	
				start.push(t,"Congratulations! You have finally reached the continent!")
				tellStory(start)
		end
		return false #ignore
	end
private
	def tellStory(story)
		@story=story
		@interface.tellStory(story)
	end
	def endLevel
		@interface.endLevel
	end
	def lostLevel
		@interface.lostLevel
	end


end