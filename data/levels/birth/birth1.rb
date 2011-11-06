class Level1<AntLevelScript
	def eventLevelStarted
		@appStory=false
		@flee=false
		if not @started
			start=StoryFlow.new("beginning")
			start.push("Rowen","Slowly you awake.")
			start.push("Rowen","You remember that you have been attacked, but ...")
			start.push("Rowen","Anything else is gone. You've lost your memory.")
			tellStory(start)
			@started=true
		end
	end

	def eventTrigger(hero,t)
		case t.name
			when "nogo"
				if hero.getPlayer.getName=="Rowen" and not @druid
					s=StoryFlow.new("noFurther")
					s.push(hero.getName,"I will not go any further! There are strangers out there. They don't seem friendly.")
					tellStory(s)
					hero.moveTo(getTarget("goback").pos)
				end
			when "druid"
				if @druid	
					if not @druid2
						@druid2=true
						s=StoryFlow.new("druid2")
						s.push("Druid","Go your path hero. I have important things to do.")
						
						tellStory(s)
					end
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
					getHero("Bantor").moveTo(getTarget("retreat").pos)
					getHero("Rowen").rest(10)

				end
			when "smith"
				if @smith==nil
					s=StoryFlow.new("smith0")
					s.push("Smith","Come here my friend!")
					tellStory(s)
					@smith=1
				end
			when "smith2"
				if @smith==1
					s=StoryFlow.new("smith1")
					s.push("Smith","Welcome to our small town! What is your name, boy?")
					s.push("Rowen","My name is .. well, I don't remember it.")
					s.push("Smith","Then he is surely the man, the druid mentioned.")
					s.push("Rowen","What do you mean?")
					s.push("Smith","Nothing. You look like a brave young man to me.")
					s.push("Smith","If you promise to act wisely and defeat those men in the north, you can have some of my men.")
					s.push("Rowen","Yes, these attacked my party - as much as this I remember.")
					s.push("Smith","Well, then recruit some of my men and defeat this group.")
					s.push("Smith","Don't forget to capture the keep, too!")
					tellStory(s)
					# assign houses to Player Rowen
					["Dwelling","Farm","Workshop"].each{|n|getHero(n).setPlayer(getPlayer("Rowen"))}
					galvador=getHero("Galvador")
					galvador.setBoss(getHero("Bantor"))
					galvador.moveTo(getHero("Bantor"))
					galvador.addHandler(:eventNewRestJob) {
						playApprenticeStory
					}
					@smith=2
				end
			when "keep"

				if hero==getHero("Rowen") and @flee==false
					# let bantor flee
					bantor=getHero("Bantor")
					bantor.setAggression(1)
					bantor.dismiss
					bantor.moveTo(AGVector2.new(40,126))
 					keep=getHero("Keep")
# 					men=bantor.getMen-[bantor]
# 					men[0..4].each{|man|
# 						man.setBoss(keep)
# 					}
	
					@flee=true
					rowen=getHero("Rowen")
					#if rowen.getJob.class==AntHeroFightJob
					rowen.attack(keep)
					#end

					# do not let rowen attack bantor anymore
					rowen.addHandler(:newJobAssigned){
 						if rowen.getJob.is_a?(AntHeroFightJob)
 							if rowen.getJob.target==bantor
# 								rowen.newHLRestJob(1)
								rowen.rest(10)
 							end
 						end
					}

				end
			when "storyFinished"
				case @story.name
					when "end"
						wonLevel
					when "smith0"
						getHero("Rowen").moveTo(getTarget("near_smith"))
						$app.hidePanel
					when "won"
						endLevel
				end
		end
		return false # ignore
	end
	def eventOwnerChanged(ent)
		case ent.getName	
			when "Rowen"
				lostLevel
			when "Bantor", "Keep"
				wonLevel
		end
	end
	def playApprenticeStory
		if @appStory==false
			@appStory=true
			@interface.focusEntity(getHero("Bantor"))
			s=StoryFlow.new("bantor")
			s.push("Bantor","Boy, why do you leave your place?")
			
			s.push("Apprentice","A stranger appeared and the people of the village want to follow him and drive you away.")
			s.push("Bantor","These fools will see their failure! You will be payed and now get off to the keep.")
			tellStory(s)
			galvador=getMap.getByName("Galvador")
			galvador.setBoss(getMap.getByName("Keep"))
		end
	end
	def wonLevel
		#super
		@won=true
		start=StoryFlow.new("won")
		start.push("","You have defeated the enemy.")
		tellStory(start)
	end
	def eventHeroDied(ent)
		case ent.getName	
			when "Rowen"
				lostLevel
				endLevel
		end
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

