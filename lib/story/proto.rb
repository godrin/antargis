#!/ usr / bin / env ruby
#
#prototype for generating stories within game
#
#(C) 2013 by David Kamphausen

require 'ostruct'
require 'pp'
require_relative 'description.rb'

def clamp(from,value,to)
  [from,value,to].sort[1]
end

FOOD=[:apple,:bread]

Event=Struct.new(:name,:params)

class Emotions<Hash
  TYPES=[:anger, :hunger, :sleepy, :active, :health]
  def initialize
    super
    TYPES.each{|t|
      self[t]=
        case t
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
      self.x+=restX.to_f*length/rest
      self.y+=restY.to_f*length/rest
      rest-length
    else
      self.x=otherPos.x
      self.y=otherPos.y
      0
    end
  end
end

Rect=Struct.new(:x,:y,:w,:h)
class Rect
  def [](position)
    position.x>=self.x && position.y>=self.y && position.x<self.x+self.w && position.y<self.y+self.h
  end
end

Circle=Struct.new(:x,:y,:r)
class Circle
  def contains(position)
    dx=position.x-self.x
    dy=position.y-self.y
    dx*dx+dy*dy<self.r**2
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

  def pick(what,amount)
    [what].flatten.each{
  | resType | if self[resType] >= amount self[resType] -=
      amount return resType end
} nil end end

    class Personality<
        Hash TYPES =
        [:aufbrausend, :nachtragend, :deltaHunger, :deltaSleepy
         , :permaDiscontent] def initialize super TYPES.each{ | t | self[t] =
                                                                  rand} self
        [:deltaHunger] = (5 + rand) / 20.0 end end

                                          Person = Struct.new(
                             : name,
                             : state,
                             : position,
                             : inventory,
                             : personality,
                             : mind,
                             : job,
                             : world,
                             : ctick) class Person FACTOR_HUNGER =
                             0.3 def tick(dt, context) return if self.state
        [:health] == 0 self.ctick || = 0 self.ctick +=
                  1 unless self.job self.job = decideOnJob(context)
#
#pp "DECIDE", self.job
                                                   end self.state
        [:hunger] = clamp(0, self.state[:hunger] + (1 + self.state[:active]) *
                                                       dt * self.personality
                          [:deltaHunger] * FACTOR_HUNGER, 1) self.state
        [:sleepy] = clamp(0, self.state[:sleepy] + (1 + self.state[:active]) *
                                                       dt * self.personality
                          [:deltaSleepy], 1) if self.state
        [:hunger]> 0.95 self
        .state[:health] = clamp(0, self.state[:health] - dt * 0.1,
                                                       1) if self.state
[:health] == 0 pp "DIE"
#exit
              end end if self.job self.job.tick(self, dt, context) self.job =
              nil if self.job.finished end end def decideOnJob(context)
                  self.mind.decideOnJob(self,
                                        context) if self.mind end def to_sx

              "xykjh" end end

                  require_relative 'jobs.rb' require_relative 'simplemind.rb'

              Place = Struct.new(
                  : name,
                  : position,
                  : inventory)

                          House = Struct.new(
                  : name,
                  : position,
                  : inventory,
                  : world)

                                      Tree =
                  Struct
                      .new(
                          : position,
                          : inventory,
                          : world) class Tree def
                      tick(dt, context) self.inventory
[:wood] += rand *dt self.inventory
[:apple] += rand *dt end end

             MapConfig =
                 Struct.new(
                           : size,
                           : randObject) class MapConfig def rand self
                     .randObject
                     .call end end

                         def
                         genPosition(mapConfig) Position
                     .new((mapConfig.rand * mapConfig.size).to_i,
                          (mapConfig.rand * mapConfig.size).to_i) end

                         def
                         genPersonName(rand)["Ban", "Gor", "Bal", "Da", "Arg",
                                             "Ol", "Gar", "Hek", "Gav"]
                     .shuffle[0] +
                 [ "dor", "in", "and", "or", "tor", "ain", "yn" ]
                     .shuffle[0] end

                         def
                         genHouseName(rand)["Bo", "Gar", "Bil", "Lerf"]
                     .sort{rand <=> 0.5}[0] +
                 [ "hin", "lot" ]
                     .sort{rand <=> 0.5}[0] end

                         def
                         gen(type, mapConfig, world) case type when
    : house House.new(genHouseName(mapConfig.rand), genPosition(mapConfig),
                      Inventory.new) when
    : person Person.new(genPersonName(mapConfig.rand), Emotions.new,
                        genPosition(mapConfig), Inventory.new, Personality.new,
                        SimpleMind.new) when
    : tree Tree.new(genPosition(mapConfig), Inventory.new) end end

          World = Struct
                      .new(
                          : config,
                          : entities) class World def
                      create(type, count = 1) count.times do self.entities
                  << gen(type, self.config, self) end end def tick(dt)
                         self.entities.each {
  | e | e.tick(dt, self) if e.respond_to ? ( : tick)
} end def getEntities(selector) self.entities.select{ | e | selector[e]} end end

    mapConfig = MapConfig.new(256, lambda{rand}) world =
    World
        .new(mapConfig, [])
#initialize
            world.create(
                     : house, 3) 5.times do world
        .create(
            : person) end world
        .create(
            : tree, 50)

            20.times do 3000.times do world.tick(0.05) end
#pp world.entities.select{ | e | e.is_a ? (Person) }
    end

    persons = world.entities.select{
        | e | e.is_a
        ? (Person) } dead = persons.select{ | p | p.state[:health] == 0} pp
                            "REST:",
        persons.length, dead.length

#pp world
