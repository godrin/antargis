
class SimpleMind
  Rule=Struct.new(:job,:condition)
  class Rule
    def when(str)
      self.condition=str
    end
  end

  class << self
    def rule(name)
      Rule.new(name)
    end
  end

  RULES=[
    rule("sleep").when(":sleepy and :athome"),
    rule("eat").when(":hunger and :hasFood"),
    rule("fetchFood").when(":hasNoFood and :well")


  ]
  def decideOnJob(person)
    RULES.each{|rule|
    }
  end
  end
