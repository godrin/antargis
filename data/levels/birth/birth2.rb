class Level2<AntLevelScript
	def eventLevelStarted
	end
	def eventTrigger(hero,t)
		case t.name
			when "nogo"
				if hero.getPlayer.class==AntHumanPlayer and not @druid
					s=StoryFlow.new("noFurther")
					s.push(hero.getName,"I will not go any further! There are strangers out there. They don't seem friendly.")
					tellStory(s)
					hero.newHLMoveJob(0,getMap.getTarget("goback").pos,0)
				end
			when "druid"
				if @druid	
					s=StoryFlow.new("druid2")
					s.push("Druid","Go your path hero.")
					tellStory(s)
				else
					@druid=true
					s=StoryFlow.new("druid1")
					s.push("Druid","Greetings stranger! I am the druid.")
					s.push("Druid","These are dangerous times. Be watchful in these woods.")
					s.push("Rowen","I think I have lost my memory.")
					s.push("Druid","Yes, but I see times of remembrance in your future...")
					s.push("Druid","You should go east and meet the smith.")
					s.push("Rowen","But there is a party of strangers on the way.")
					s.push("Druid","They will be gone soon.")
					tellStory(s)
					getMap.getByName("Bantor").newHLMoveJob(0,getMap.getTarget("retreat").pos,0)
				end
			when "storyFinished"
		
				if @story.name=="end" or @story.name=="marcus1"
					wonLevel
					endLevel
				end
		end
		return false # ignore
	end
	def eventOwnerChanged(ent)
		case ent.getName	
			when "Rowen"
				lostLevel
			when "Bantor"
				s=StoryFlow.new("bantor")
				s.push("Rowen","We have indeed defeated Bantor. But some foes are still left within the tower.")
				s.push("Rowen","These must be defeated, too.")
				tellStory(s)
#				wonLevel
			when "Keep"
				rowen=getMap.getByName("Rowen")
				#raise 1
				rowen.newHLMoveJob(0,getMap.getTarget("near_tower").pos,0)
				rowen.addHandler(:eventHLJobFinished){
					if (rowen.getPos2D-getMap.getTarget("near_tower").pos).length<1
						sendMarcus
					else
						rowen.newHLMoveJob(0,getMap.getTarget("near_tower").pos,0)
					end
				}
				$app.hidePanel
		end
	end
	def sendRowen
	end
	def sendMarcus
		marcus=getMap.getByName("Marcus")
		marcus.newMoveJob(0,getMap.getByName("Rowen").getPos2D,2)
		$app.hidePanel
		marcus.addHandler(:eventJobFinished){
			s=StoryFlow.new("marcus1")
			s.push("Marcus","I have a message for you.")
			s.push("Rowen","Speak! What is it?")
			s.push("Marcus","You seem to be against the new lords, so I think can trust you.")
			s.push("Marcus","King Garon has been captured and Duke Dengal wants to enthrone himself.")
			s.push("Rowen","This is very bad news.")
			s.push("Marcus","It may get worse. Word came through that someone is starting a rebellion here in Green Hills.")
			s.push("Marcus","You must flee into the hills.")
			s.push("Rowen","And what will you do?")
			s.push("Marcus","I'll stay behind and join with you later on.")

			tellStory(s)
		}
	end
	def eventHeroDied(ent)
		case ent.getName	
			when "Rowen"
				lostLevel
				endLevel
		end
	end
private
	def wonLevel
		#super
		@won=true
		start=StoryFlow.new("won")
		start.push("","You have defeated the enemy.")
		tellStory(start)
	end
	def tellStory(story)
		@story=story
		@interface.tellStory(story)
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
	def endLevel
		@interface.endLevel
	end

end

