<a name="readme-top"></a>

<!-- ABOUT THE PROJECT -->
## About The Project

![App screenshot](https://github.com/MZdzw/QualTask/tree/main/img/app.png?raw=true)

Hi! You got to the program which calculates the CPU usage based on /proc/stat file. This program is runnable only on linux OS.


<p align="right">(<a href="#readme-top">back to top</a>)</p>


<!-- GETTING STARTED -->
## Getting Started

Below there are instructions and prerequisites how to "compile" and run this program.

### Prerequisites

Below are listed units needed to compile the project.
* make
  ```sh
  make --version
  ```
* gcc or clang
  ```sh
  gcc --version
  clang --version
  ```
* export CC environment variable (not necessary step)
  ```sh
  export CC="gcc"
  ```
  or
  ```sh
  export CC="clang"
  ```
  To get ensured print CC environment variable
  ```sh
  printenv CC
  gcc
  ```

### Installation

To run this app you need to follow these steps:

1. Clone the repo
   ```sh
   git clone https://github.com/MZdzw/QualTask.git
   ```
2. Change directory to the cloned repositry
   ```sh
   cd <cloned repository directory>
   ```
3. "Compile" and link all the files
   ```sh
   make all
   ```
4. If compilation succeded change directory to app file
   ```sh
   cd build/bin
   ```
5. Run application
   ```sh
   ./qualTask.out
   ```

<p align="right">(<a href="#readme-top">back to top</a>)</p>



<!-- USAGE EXAMPLES -->
## Usage

After running the program it will work forever. To end this process you need to open second terminal (CTRL+ALT+T) and generate SIGTERM. You can achieve that by running following commands
* List user processes
  ```sh
  ps a
  ```
Example output should look like this
  ```sh
   PID  TTY      STAT   TIME COMMAND
    [...]
   4278 pts/1    Ss     0:00 bash
   7106 pts/2    Ss     0:00 bash
   7116 pts/1    Sl+    0:00 ./qualTask.out
   7561 pts/2    R+     0:00 ps a
  ```
* Send SIGTERM
  ```sh
  kill <PID>
  ```
  in this example it will be:
  ```sh
  kill 7116
  ```
After that the app should end its performance.


<p align="right">(<a href="#readme-top">back to top</a>)</p>
