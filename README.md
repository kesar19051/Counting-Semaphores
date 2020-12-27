# CountingSemaphores

The blocking and non-blocking versions of semaphores have been implemented using the conditional mutex.
One more variable is used for the implementation which is called the wakeups. It counts the number of pending signals.

Using the implemented semaphores dining philosophers problem has been solved.
This is a modified version of dining philosophers where there are k philosophers and two bowls to eat from.
Both the bowls should be available for a philosopher to eat.

The deadlock condition is taken care of by providing the even numbered philosophers first the left fork and then the right fork, and the odd numbered philosophers first the right fork and then the left fork.
