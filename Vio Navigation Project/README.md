Monocular Visual-Inertial Odometry (VIO) Estimator
A from-scratch implementation of visual-inertial odometry in Python — the technique drones, AR/VR headsets, and mobile robots use to track their own position and orientation by fusing a camera with an IMU, without relying on GPS.

Core idea: an IMU gives you fast, high-rate motion estimates that drift quickly; a camera gives you drift-free but slower, noisier corrections. Fusing them well (via an Extended Kalman Filter) gets you the best of both.


Status
Component
Status
IMU pre-integration
✅ Implemented, unit-tested
Error-state EKF (predict + update)
✅ Implemented, unit-tested, validated end-to-end
Visual front end (FAST/KLT/RANSAC)
✅ Implemented, working
Synthetic dataset generator
✅ Implemented (see Dataset)
ATE / RPE evaluation
✅ Implemented from scratch, matches evo conventions
Baseline accuracy results
✅ Documented — see PHASE1_REPORT.md
MSCKF sliding-window update
🚧 In progress, not yet stable — see Known Limitations
C++ port
⬜ Not started


This project is under active development. The sections below describe what's actually built and validated today, not an aspirational end state.


Why this exists
Built to learn and demonstrate the full VIO pipeline end-to-end: IMU mechanization, sensor fusion via Kalman filtering, sparse visual tracking, and rigorous accuracy evaluation — plus, just as important, the diagnostic process of finding out why an estimator drifts rather than treating accuracy numbers as a black box. See PHASE1_REPORT.md for a full write-up of that investigation.


Architecture
IMU samples (200Hz)  ──▶  Pre-integration  ──▶  ┐

                                                  ├─▶  Error-State EKF  ──▶  Pose estimate

Camera frames (20Hz) ──▶  FAST/KLT/RANSAC  ──▶  ┘        (predict + update)

                           feature tracking

src/imu_preintegration.py — accumulates IMU measurements between camera frames into a single relative-motion measurement, with first-order Jacobians so it can be cheaply re-corrected if the bias estimate changes later, instead of re-integrating raw samples.
src/ekf.py — the core error-state EKF. Tracks a 15-dimensional state (position, velocity, orientation, gyro/accel bias), with orientation kept in its tangent space (rotation vector) rather than as a quaternion, to avoid a singular covariance.
src/feature_tracker.py — FAST corner detection, pyramidal Lucas-Kanade (KLT) tracking frame-to-frame, RANSAC-based outlier rejection via essential-matrix estimation.
src/triangulation.py — two-view and multi-view (linear DLT) triangulation, used to turn tracked 2D features into 3D landmarks.
src/metrics.py — Absolute Trajectory Error (Umeyama-aligned) and Relative Pose Error, implemented from scratch.


Getting Started
python3 -m venv venv

source venv/bin/activate       # Windows: venv\Scripts\activate

pip install -r requirements.txt

python tests/sanity_checks.py  # should print 5x PASS
Generate a dataset and run the pipeline
Public VIO benchmark datasets (EuRoC MAV, TUM VI) had unreliable hosting during development, so a synthetic dataset generator is included — it produces a trajectory with exact, analytically-known ground truth, in the same file format the EuRoC-style loader expects, so nothing else in the pipeline needs to change if you later switch to real data.

python scripts/generate_synthetic_dataset.py --out data/synthetic_circle --duration 20

python scripts/run_pipeline.py data/synthetic_circle outputs/est.tum

python scripts/evaluate.py data/synthetic_circle outputs/est.tum


Results
Current baseline (Phase 1, EKF-only, on a 20-second synthetic trajectory — see PHASE1_REPORT.md for full methodology, diagnostic investigation, and reproduction steps):

Metric
Value
ATE RMSE
3.42 m
RPE Trans RMSE (1-frame)
0.35 m
RPE Rot RMSE (1-frame)
0.30°


This is an honestly-documented first-pass baseline, not a state-of-the-art result. The report walks through a structured root-cause investigation (three isolated diagnostic experiments) that identifies exactly why the estimator drifts over longer runs, rather than leaving it as an unexplained number.


Known Limitations & Roadmap
Landmark triangulation uses the filter's own pose estimate, creating a feedback loop that limits long-run accuracy (root-caused in PHASE1_REPORT.md, Section 6). An MSCKF-style sliding-window update — which avoids this by algebraically removing the landmark from the update via a null-space projection — is in progress in src/msckf.py but not yet stable; the core math (state augmentation, marginalization, null-space update) is individually unit-tested and correct, but full-pipeline integration still diverges and the remaining bug hasn't been isolated yet. Contributions welcome.
IMU mechanization uses first-order Euler integration, not RK4.
Validated only against synthetic data — real camera images (rolling shutter, motion blur, lighting variation) will surface new failure modes.
Monocular scale is only weakly observable, as expected for a monocular (non-stereo) setup.
C++ port not started. Planned approach: port utils.py and ekf.py::predict() to Eigen first (validating against Python output at each step), keep dataset loading/plotting/metrics in Python.


Project Structure
src/

  dataset.py               # EuRoC-format dataset loader

  dataset_tumvi.py          # TUM VI variant (separate calibration handling)

  utils.py                   # SO(3) math, pinhole projection + Jacobians

  imu_preintegration.py      # IMU pre-integration between frames

  feature_tracker.py         # FAST + KLT + RANSAC visual front end

  triangulation.py            # Two-view and multi-view triangulation

  ekf.py                       # Core error-state EKF (Phase 1 estimator)

  msckf.py                     # Sliding-window MSCKF update (WIP, unstable)

  metrics.py                     # ATE / RPE evaluation

scripts/

  generate_synthetic_dataset.py   # Ground-truth-exact synthetic data generator

  run_pipeline.py                   # End-to-end run using the EKF (stable)

  run_pipeline_msckf.py              # End-to-end run using MSCKF (WIP)

  evaluate.py                         # Compute ATE/RPE from a saved trajectory

tests/

  sanity_checks.py                     # Core-math unit tests

PHASE1_REPORT.md                        # Full baseline results + diagnostics

PORTFOLIO_SUMMARY.md                     # Project summary for resumes/portfolios


Testing
python tests/sanity_checks.py

Covers: static-IMU rest check, constant-acceleration kinematics, pre-integration vs. stepwise-EKF agreement, ATE self-consistency, and landmark-update convergence. Run this after touching any of the core math before trusting a full pipeline run.


References
Mourikis, A. I., & Roumeliotis, S. I. (2007). A Multi-State Constraint Kalman Filter for Vision-aided Inertial Navigation. ICRA.
Forster, C., Carlone, L., Dellaert, F., & Scaramuzza, D. (2017). On-Manifold Preintegration for Real-Time Visual-Inertial Odometry. IEEE Transactions on Robotics.
Solà, J. (2017). Quaternion kinematics for the error-state Kalman filter. (reference for the error-state formulation used here)
