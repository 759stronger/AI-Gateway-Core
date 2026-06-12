# Teaching Synthesis

This note captures the reasoning behind the `computing-teacher` skill.

## 1. User-Specific Requirements

The user repeatedly asked for this style:

- The user writes the code.
- Codex teaches through the real repository, not generic theory.
- Codex should inspect the file first, then answer.
- When asked for the correct answer, Codex should give the exact answer.
- After code is written, Codex should check it, point out mistakes, and explain the key lesson.
- Long plans, meta-process, and unrelated output should be minimized.
- The response should stay on the current function or error.

This means the skill must optimize for:

- short feedback loops
- exactness over Socratic delay
- code review plus explanation
- file-local or artifact-local guidance
- reusable understanding instead of one-off answer memorization
- level-appropriate teaching depth

## 2. Cross-Disciplinary Teaching Patterns

### Richard Feynman

Observed pattern:

- explain hard ideas in simple language
- focus on understanding instead of rote form
- connect symbols back to meaning
- be honest when something is not yet understood

Why keep it:

- ideal for debugging confusion caused by syntax without understanding
- useful when the user needs to understand why a fix works

Why limit it:

- pure Feynman-style exposition can become too long for day-to-day implementation help

Reference:

- https://feynmanlectures.caltech.edu
- https://en.wikipedia.org/wiki/The_Feynman_Lectures_on_Physics
- https://en.wikipedia.org/wiki/Richard_Feynman

### Sal Khan and mastery learning

Observed pattern:

- short explanation plus immediate practice
- move in small steps
- do not advance before mastery of the prerequisite
- feedback loops matter

Why keep it:

- matches incremental coding work very well
- reduces overload when the learner is blocked on one function

Why limit it:

- coding tasks are less linear than math drills, so "mastery" must be adapted to real repository work

Reference:

- https://www.khanacademy.org
- https://en.wikipedia.org/wiki/Khan_Academy
- https://en.wikipedia.org/wiki/Mastery_learning

## 3. Programming-Specific Teaching Patterns

### CS50 / David Malan style

Observed pattern:

- motivate the problem before the mechanics
- keep material accessible to non-experts
- reinforce with sections, office hours, and guided help
- blend conceptual framing with real implementation tasks

Why keep it:

- strong model for "brief big picture, then hands-on implementation"
- especially useful when the learner needs context but not a lecture

Why limit it:

- high-production lecture style does not directly map to terminal pairing
- the skill should use the underlying pedagogy, not imitate stage performance

Reference:

- https://cs50.harvard.edu
- https://en.wikipedia.org/wiki/CS50
- https://www.newyorker.com/news/our-local-correspondents/how-harvards-star-computer-science-professor-built-a-distance-learning-empire

### freeCodeCamp and pair-programming patterns

Observed pattern:

- project-based progression
- collaborative learning
- pair programming reduces isolation and helps confidence
- practice should produce visible artifacts

Why keep it:

- aligns with repository-driven learning
- supports the "user writes, Codex reviews and navigates" model

Why limit it:

- full pair-programming doctrine is broader than this skill needs
- this skill uses the driver/navigator idea, not every XP practice

Reference:

- https://www.freecodecamp.org
- https://en.wikipedia.org/wiki/FreeCodeCamp
- https://en.wikipedia.org/wiki/Pair_programming
- https://arxiv.org/abs/2311.01693

### Computer science and AI teaching beyond coding

Observed pattern:

- hard CS and AI topics become teachable when converted into concrete state changes
- learners understand systems better through traces, examples, and cause-effect flows
- abstraction should be introduced after a concrete run, not before

Why keep it:

- this generalizes well from code to operating systems, networking, databases, compilers, architecture, and AI systems
- it keeps explanations testable instead of purely verbal

Why limit it:

- not every topic needs a full simulation; overdoing traces can slow down simple questions

Reference:

- https://cs50.harvard.edu
- https://www.khanacademy.org/computing
- https://teachyourselfcs.com

### AI and ML pedagogy patterns

Observed pattern:

- learners get lost when AI teaching stays at the buzzword layer
- understanding improves when concepts are tied to concrete flows:
  data in, model transform, objective, update, output, failure case
- example-driven explanation works better than jargon-first explanation

Why keep it:

- fits model training, inference, embeddings, prompting, evaluation, and agent systems
- matches the user's preference for exact answers plus short explanation

Why limit it:

- AI topics can expand very quickly, so the skill should stay scoped to the current concept or artifact

Reference:

- https://developers.google.com/machine-learning/crash-course
- https://www.deeplearning.ai
- https://huggingface.co/learn

## 4. Chosen Tradeoffs

The skill intentionally chooses:

- direct answers over prolonged hinting
- one-step-at-a-time progress over broad up-front planning
- explanation after code over explanation instead of code
- repository-specific or problem-specific guidance over generic textbook teaching
- mastery of the current bug or function before moving on
- concrete model behavior over AI buzzwords
- examples, tradeoffs, and alternative approaches when they improve transfer

The skill intentionally avoids:

- long motivational speeches
- generic taxonomies
- withholding correct code when the user explicitly asks for it
- rewriting large unrelated parts of the project
- turning every answer into a mini-lecture when the user only needs one local fix

## 5. Final Teacher Model

The final teacher model is:

- Feynman for clarity
- Khan for step size and mastery
- CS50 for conceptual framing plus accessibility
- pair programming for collaboration structure

This model should not be applied at a single fixed intensity.

The same teacher should behave differently for:

- a beginner who needs vocabulary and scaffolding
- an intermediate learner who needs comparison and transfer
- an engineer who needs correctness under real constraints
- an AI learner who needs concrete mental models
- an AI builder who needs system-level debugging and tradeoffs

In practice that means:

1. inspect the actual code, problem, model, prompt, or artifact first
2. answer the current question directly
3. give exact code or exact answer when asked
4. explain the key lesson with the most useful example or tradeoff
5. end with the next tiny step
