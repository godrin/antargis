
class SimpleMind
    Rule = Struct
               .new(
                   : job,
                   : jobOptions,
                   : condition) class Rule def
               when(str) self.condition = str self end end

                                          class Cond def
                                          checkCond(cond, person,
                                                    context) if cond.respond_to
    ? (
          : [])cond[person, context] elsif cond.is_a
    ? (Proc)cond.call(person, context) elsif cond.is_a
    ? (Symbol)person.state[cond] else raise "invalid condition",
    cond

            end end end

        class Not <
        Cond def initialize(cond) @cond =
        cond end def[](person, context) 1 -
            checkCond(@cond, person, context) end end

            class AndCond <
        Cond def initialize(*args) @conds =
            args end def[](person, context)
                @conds.map{ | cond | checkCond(cond, person, context)}
                    .inject(1){ | a, b | a *b} end end

            class
            << self def
               rule(jobName, options = {}) Rule
                   .new(jobName, options) end def
               _and(*args) AndCond.new(*args) end def
               _not(cond) Not.new(cond) end def
               _has(what) what = [ what ].flatten lambda{
                | person,
                context | what.map{ | res |
                                    2 * Math.atan(person.inventory[res]) /
                                        Math::PI}
                              .max ||
                    0} end def _sleepy lambda{
                | person,
                context | person.state[:sleepy]} end def _hungry lambda{
                | person, context | person.state[:hunger]} end def _well lambda{
                | person,
                context | person.state[:health]} end def _athome lambda{
                | person, context | 1} end def _seldom lambda{
                | person, content | 0.3} end end

                                 include Job RULES =
                [
                  rule(Sleep).when(_sleepy),
#rule(Sleep).when(_and(_sleepy, _athome)),
                  rule(Eat).when(_and(_hungry, _has(FOOD))),
                  rule(Fetch, { : what = > FOOD})
                      .when(_and(_not(_has(FOOD)), _well)),
                  rule(Rest).when(_seldom)
                ]

    def decideOnJob(person, context) bestRule = RULES.max_by {
  | rule | rule.condition[person, context]
}
#pp "BEST", bestRule, person
puts "New JOB #{bestRule.job} #{person.state.inspect}" bestRule.job.new(
    bestRule.jobOptions) end

    end
