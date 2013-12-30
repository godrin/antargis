
module Job

  Base0=Struct.new(:options,:finished)
  class Base<Base0
    def initialize(*args)
      super(*args)
      puts "New JOB #{self.class} #{self.options.inspect}"
    end

    def _and(*args)
      lambda{|e|args.select{|lmbda|not lmbda[e]}.length==0}
    end
  end

  class Rest<Base
    def initialize(*args)
      super(*args)
      @length=2
    end
    def tick(person,dt,content)
      @length-=dt
      if person.state[:health]<1
        person.state[:health]=clamp(0,person.state[:health]+dt*0.5,1)
      end
      if @length<0
        self.finished=true
      end
    end
  end

  class Eat<Base
    def initialize(ops)
      @what=nil
      @amount=0
    end
    def tick(person,dt,context)
      # FIXME
      unless @what
        @what=person.inventory.pick(FOOD,1)
        if @what
          @amount=2
        end
      end

      d=[@amount,dt].min

      person.state[:hunger]-=d*2*person.personality[:deltaSleepy]
      @amount-=d
      if not @amount>0
        self.finished=true
      end
    end
  end
  class Sleep<Base
    def tick(person,dt,context)
      s=person.state[:sleepy]
      s=[0,s-dt*4*person.personality[:deltaSleepy]].max
      person.state[:sleepy]=s
      if s==0
        self.finished=true
      end
    end
  end

  module MoveModule
    def speed
      5.0
    end
    def move(targetPos,person,dt,context)
      unless @moveFinished
        rest=person.position.moveTo(targetPos,dt*speed)
        @moveFinished=(rest<=0)
      end
      !@moveFinished
    end
    def moveFinished
      @moveFinished
    end
  end

  module LookModule
    def hasInventory(whatArray)
      lambda{|e| e.inventory.select{|k,v|whatArray.member?(k) and v>0}.length>0}
    end
    def somethingNear(position,radius,context,selector)
      circle=Circle.new(position.x,position.y,radius)
      context.getEntities(lambda{|e|circle.contains(e.position) and selector[e]})
    end
  end

  class Find<Base
    include MoveModule
    include LookModule
  end

  class Fetch<Base
    include MoveModule
    include LookModule
    def initialize(ops)
      @what=ops[:what]
      @fromWhere=nil
    end
    def notEqual(person)
      lambda{|e|e!=person}
    end
    def tick(person,dt,context)
      @fromWhere||=somethingNear(person.position,500,context,_and(hasInventory(@what),notEqual(person)) )[0]
      if @fromWhere
        unless move(@fromWhere.position,person,dt,context)
          take(person,@fromWhere,@what)
        end
      end
    end
    def take(person,fromWhere,what)
      resType=fromWhere.inventory.select{|k,v|what.member?(k) and v>0}.keys[0]
      if resType
        amount=[1,fromWhere.inventory[resType]].min
        person.inventory[resType]+=amount
        fromWhere.inventory[resType]-=amount
        self.finished=true
      end
    end
  end
end
