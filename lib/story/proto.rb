#!/usr/bin/env ruby
#
# prototype for generating stories within game
#
# (C) 2013 by David Kamphausen

require 'ostruct'
require 'pp'
require_relative 'description.rb'

Event=Struct.new(:name)

class Emotions<Hash
  TYPES=[:anger, :hunger, :sleepy, :active, :health]
  def initialize
    super
    TYPES.each{|t|
      self[t]=case t
              when :health
                1
              else
                0
              end
    }
  end
end

class Assocs<Hash
  TYPES=[:friend, :debt, :afraid]
  def initialize
    super
    TYPES.each{|t|self[t]=0}
  end
end

Position=Struct.new(:x,:y)
class Position
  def moveTo(otherPos,length)
    restX=otherPos.x-self.x
    restY=otherPos.y-self.y
    rest=Math::sqrt(restX*restX+restY*restY)
    if rest>length
      self.x+=restX*length/rest
      self.y+=restY*length/rest
      0
    else
      self.x=otherPos.x
      self.y=otherPos.y
      length-rest
    end
  end
end

ThingType=Struct.new(:name)

Thing=Struct.new(:type)

class Inventory<Hash
  TYPES=[:boat,:sword,:bread,:apple,:wood,:stone,:ore]
  def initialize
    super
    TYPES.each{|t|self[t]=0}
  end
end

class Personality<Hash
  TYPES=[:aufbrausend,:nachtragend,:deltaHunger,:deltaSleepy,:permaDiscontent]
  def initialize
    super
    TYPES.each{|t|self[t]=rand}
    self[:deltaHunger]=(1+rand)/20.0
  end
end

Person=Struct.new(:name,:state,:position, :inventory,:personality,:mind,:job,:world)
class Person
  def tick(dt,context)
    self.state[:hunger]+=(1+self.state[:active])*dt*self.personality[:deltaHunger]
    self.state[:sleepy]+=(1+self.state[:active])*dt*self.personality[:deltaSleepy]

    self.job=decideOnJob(context) unless self.job 
    if self.job
      self.job.tick(self,dt,context)
      self.job=nil if self.job.finished
    end
  end
  def decideOnJob(context)
    self.mind.decideOnJob(self,context)  if self.mind
  end
  def to_sx

    "xykjh"
  end
end

require_relative 'jobs.rb'
require_relative 'simplemind.rb'

Place=Struct.new(:name,:position, :inventory)

House=Struct.new(:name,:position, :inventory,:world)

Tree=Struct.new(:position,:inventory,:world)
class Tree
  def tick(dt,context)
    self.inventory[:wood]+=rand*dt
  end
end

MapConfig=Struct.new(:size,:randObject)
class MapConfig
  def rand
    self.randObject.call
  end
end


def genPosition(mapConfig)
  Position.new((mapConfig.rand*mapConfig.size).to_i,
               (mapConfig.rand*mapConfig.size).to_i)
end

def genPersonName(rand)
  ["Ban","Gor","Bal","Da","Arg","Ol","Gar","Hek","Gav"].shuffle[0]+
    ["dor","in","and","or","tor","ain","yn"].shuffle[0]
end

def genHouseName(rand)
  ["Bo","Gar","Bil","Lerf"].sort{rand<=>0.5}[0]+
    ["hin","lot"].sort{rand<=>0.5}[0]
end

def gen(type,mapConfig,world)
  case type
  when :house
    House.new(genHouseName(mapConfig.rand),genPosition(mapConfig),Inventory.new)
  when :person
    Person.new(genPersonName(mapConfig.rand),Emotions.new,genPosition(mapConfig),Inventory.new,Personality.new,SimpleMind.new)
  when :tree
    Tree.new(genPosition(mapConfig),Inventory.new)
  end
end

World=Struct.new(:config, :entities)
class World
  def create(type,count=1)
    count.times do
      self.entities<<gen(type,self.config,self)
    end
  end
  def tick(dt)
    self.entities.each{|e|
      e.tick(dt,self) if e.respond_to?(:tick)
    }
  end
end

mapConfig=MapConfig.new(256,lambda{rand})
world=World.new(mapConfig,[])
# initialize
world.create(:house,3)
world.create(:person)
world.create(:tree,50)

20.times do 
  world.tick(0.05)
  #world.createHouse
  pp world.entities.select{|e|e.is_a?(Person)} #genHouse(mapConfig)
end



