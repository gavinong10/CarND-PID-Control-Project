# CarND-Controls-PID
Self-Driving Car Engineer Nanodegree Program

---

## Reflection

### Describe the effect each of the P, I, D components had in your implementation.
As per the theory, the components had the respective impact on driving:
- P: 'Proportion' component was what initiated a correction in direct proportion to the cross track error. This was especially important for making corners, where the curvature of the road would quickly introduce cross track error and required an immediate response from the vehicle to follow the track.
- I: 'Integration' component was the correction introduced to counteract steering bias. If there was a steering bias, then the proportional component would only visibly respond and make a correction once it has veered off with a high enough CTE. The need for the I component can manifest in multiple ways - either the car fails to fully restore back to the intended path given a steady state (it remains off track by a constant factor), or it oscillates favoring one side. There wasn't much observed steering bias in this case, but this is still an important parameter for the vehicle to restore to the center of the road during turns as the P component is insufficient on its own. It would result in the vehicle hugging the outer parts of the road.
- D: 'Derivative' component was the counter-correction responding to the tendency for corrections to overshoot. Its presence caused an oscilating vehicle to stabilize - however, when tuned too high, it could cause the controller to make corrections in a very jerky fashion.

After optimization, the PID parameters were set to p = 0.12, i = 0.00038 and d = 0.0375.

A video explaining the performance and showing the vehicle in action can be found [here at this Youtube link](https://youtu.be/Qj-NxDb-MaM)

### Describe how the final hyperparameters were chosen.
Initially, a sort of Twiddle variation was coded. It was assumed that we could find a set of parameters that would stay on the road for a long duration, and then modify it slightly every few seconds based on the error accumulated and slowly converge on a solution. This was perhaps overly ambitious, as the code tried to compensate for the inconsistency in different starting points, road sections etc. through various techniques including averaging and sampling. In the end, it proved difficult to find a set of parameters to begin with that would stay on the road long enough to be able to fine tune.

Subsequently, it was apparent that understanding the parameters individually was key. Fast feedback was important, so the code was modified so that parameters could be changed for the PID filter instantaneously as the car was in motion (and without rebuilding the code). Starting with small P, I, D values, D was raised to a level that stabilized the car on the straight, followed by raising P so that corners could be made adequately. D was then further tweaked in conjunction with adjusting P to finally converge on the solution. I was left at a small value, only to be slightly tweaked at the end to allow the vehicle to better restore itself to the center of the road around corners.
## Dependencies

* cmake >= 3.5
 * All OSes: [click here for installation instructions](https://cmake.org/install/)
* make >= 4.1
  * Linux: make is installed by default on most Linux distros
  * Mac: [install Xcode command line tools to get make](https://developer.apple.com/xcode/features/)
  * Windows: [Click here for installation instructions](http://gnuwin32.sourceforge.net/packages/make.htm)
* gcc/g++ >= 5.4
  * Linux: gcc / g++ is installed by default on most Linux distros
  * Mac: same deal as make - [install Xcode command line tools]((https://developer.apple.com/xcode/features/)
  * Windows: recommend using [MinGW](http://www.mingw.org/)
* [uWebSockets](https://github.com/uWebSockets/uWebSockets)
  * Run either `./install-mac.sh` or `./install-ubuntu.sh`.
  * If you install from source, checkout to commit `e94b6e1`, i.e.
    ```
    git clone https://github.com/uWebSockets/uWebSockets 
    cd uWebSockets
    git checkout e94b6e1
    ```
    Some function signatures have changed in v0.14.x. See [this PR](https://github.com/udacity/CarND-MPC-Project/pull/3) for more details.
* Simulator. You can download these from the [project intro page](https://github.com/udacity/self-driving-car-sim/releases) in the classroom.

There's an experimental patch for windows in this [PR](https://github.com/udacity/CarND-PID-Control-Project/pull/3)

## Basic Build Instructions

1. Clone this repo.
2. Make a build directory: `mkdir build && cd build`
3. Compile: `cmake .. && make`
4. Run it: `./pid`. 

## Editor Settings

We've purposefully kept editor configuration files out of this repo in order to
keep it as simple and environment agnostic as possible. However, we recommend
using the following settings:

* indent using spaces
* set tab width to 2 spaces (keeps the matrices in source code aligned)

## Code Style

Please (do your best to) stick to [Google's C++ style guide](https://google.github.io/styleguide/cppguide.html).

## Project Instructions and Rubric

Note: regardless of the changes you make, your project must be buildable using
cmake and make!

More information is only accessible by people who are already enrolled in Term 2
of CarND. If you are enrolled, see [the project page](https://classroom.udacity.com/nanodegrees/nd013/parts/40f38239-66b6-46ec-ae68-03afd8a601c8/modules/f1820894-8322-4bb3-81aa-b26b3c6dcbaf/lessons/e8235395-22dd-4b87-88e0-d108c5e5bbf4/concepts/6a4d8d42-6a04-4aa6-b284-1697c0fd6562)
for instructions and the project rubric.

## Hints!

* You don't have to follow this directory structure, but if you do, your work
  will span all of the .cpp files here. Keep an eye out for TODOs.

## Call for IDE Profiles Pull Requests

Help your fellow students!

We decided to create Makefiles with cmake to keep this project as platform
agnostic as possible. Similarly, we omitted IDE profiles in order to we ensure
that students don't feel pressured to use one IDE or another.

However! I'd love to help people get up and running with their IDEs of choice.
If you've created a profile for an IDE that you think other students would
appreciate, we'd love to have you add the requisite profile files and
instructions to ide_profiles/. For example if you wanted to add a VS Code
profile, you'd add:

* /ide_profiles/vscode/.vscode
* /ide_profiles/vscode/README.md

The README should explain what the profile does, how to take advantage of it,
and how to install it.

Frankly, I've never been involved in a project with multiple IDE profiles
before. I believe the best way to handle this would be to keep them out of the
repo root to avoid clutter. My expectation is that most profiles will include
instructions to copy files to a new location to get picked up by the IDE, but
that's just a guess.

One last note here: regardless of the IDE used, every submitted project must
still be compilable with cmake and make./
