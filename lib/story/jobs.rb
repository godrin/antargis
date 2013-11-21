
module Job

  Base0=Struct.new(:options,:finished)
  class Base<Base0
    def initialize(*args)
      super(*args)
      puts "New JOB #{self.class} #{self.options.inspect}"
    end
  end
  class Eat<Base
    def tick(person,dt,context)
      # FIXME
      person.state[:hunger]-=dt*2*person.personality[:deltaSleepy]
    end
  end
  class Sleep<Base
    def tick(person,dt,context)
      person.state[:sleepy]-=dt*2*person.personality[:deltaSleepy]
    end
  end

  class MoveBase<Base
    def speed
      5.0
    end
    def move(targetPos,person,dt,context)
      rest=person.position.moveTo(targetPos,dt*speed)
      if rest<=0
        self.finished=true
      end
    end
  end

  class Find<MoveBase
    def somethingNear(radius,context,selector)
    end
  end

  class Fetch<MoveBase
    def tick(person,dt,context)
      move(Position.new(0,0),person,dt,context)
    end
  end
end
