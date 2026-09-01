Anaya Scott — Project Portfolio
Electrical Engineering student at the University of Houston working toward a career in robotics and autonomous systems. This page indexes my active and past project work — embedded systems, sensor fusion, navigation software, and course-based engineering projects.

📫 anscott@cougarnet.uh.edu · LinkedIn · GitHub


Projects
Project
Focus
Status
Resilient Visual-Inertial Navigation Stack for UAS
VIO/SLAM, sensor fusion
🟢 Active — Phase 1
ARM Cortex-M4 Microprocessor Systems
Embedded systems
✅ Complete
University Rover Challenge (URC)
Robotics, sensors
🟢 Active
The ECE Archive
Open-source / documentation
🟢 Active
HUNCH Project Intern — NASA JSC
Aerospace prototyping
✅ Complete



Resilient Visual-Inertial Navigation Stack for UAS
Independent project · Aug 2026 – Present

Building a real-time visual-inertial odometry (VIO) estimator for small UAS that fuses camera and IMU data to maintain accurate pose estimates when GNSS is degraded or unavailable. Currently in Phase 1 (Estimator Core):

Implementing IMU pre-integration, an ORB/FAST-based visual front end with RANSAC outlier rejection, and an EKF update step
Validating against the public EuRoC MAV and TUM VI VIO benchmark datasets
Porting the estimator's core loop from Python to C++ for performance, with accuracy tracked via standard trajectory error metrics (ATE, RPE)

Stack: Python, C++, OpenCV, EKF/filterpy


ARM Cortex-M4 Microprocessor Systems
ECE 3436 Coursework · Jan 2026 – May 2026

Two semesters of embedded systems coursework centered on register-level ARM Cortex-M4 assembly programming for the TI TM4C123 LaunchPad — GPIO and SysTick peripheral control, memory-mapped data storage, and a from-scratch register-only statistics program (no memory access, pure SDIV/MUL arithmetic).

Stack: ARM Cortex-M4 Assembly, Keil MDK-ARM/µVision

Repo: ECE-Archive-files / ECE 3436 MicroProcessing


University Rover Challenge (URC)
Embedded Sub-Team

Researching and evaluating electrical components and sensors for a Mars rover navigation system as part of an international robotics competition team. Tested and compared 2D LiDAR sensors for mapping applications to support autonomous navigation, collaborating across mechanical, software, and embedded sub-teams.

Stack: LiDAR sensing, embedded systems, cross-functional hardware design


The ECE Archive
Open-Source Course Repository · 2024 – Present

A public GitHub repository I built and maintain, organizing 650+ files of notes, labs, exams, and project material across 7 undergraduate ECE courses. Used as a study reference by fellow students; I manage it as an actively maintained, collaborative project — reviewing contributor pull requests and issues.

Stack: Git/GitHub, version control, technical documentation

Repo: github.com/Buggy-Scott/ECE-Archive-files


HUNCH Project Intern — NASA JSC
NASA – Johnson Space Center · Aug 2019 – May 2021

Designed a lightweight, high-temperature-tolerant prototype for an ISS re-entry payload, judged by NASA engineers. Incorporated mentor feedback across design iterations, using CAD and 3D printing for rapid prototyping.

Stack: CAD (SolidWorks, AutoCAD), 3D printing, rapid prototyping



Last updated: August 2026

References
Mourikis, A. I., & Roumeliotis, S. I. (2007). A Multi-State Constraint Kalman Filter for Vision-aided Inertial Navigation. ICRA.
Forster, C., Carlone, L., Dellaert, F., & Scaramuzza, D. (2017). On-Manifold Preintegration for Real-Time Visual-Inertial Odometry. IEEE Transactions on Robotics.
Solà, J. (2017). Quaternion kinematics for the error-state Kalman filter. (reference for the error-state formulation used here)
