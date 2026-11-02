from asyncio import threads
import os
import sys
import subprocess
import os.path
import fileinput
import re
import time
import shutil
import Globals
import threading

EX_OK = getattr(os, "EX_OK", 0)


class ModelRunner:
    def __init__(self):
        self.result_container = {}

        self.executionTypeList = []
        self.executionTypeList.append(["TwoSex", "betadiff"])
        self.executionTypeList.append(["TwoSexHybridMortality", "betadiff"])
        self.executionTypeList.append(["SBW", "betadiff"])
        self.executionTypeList.append(["Simple", "betadiff"])
        self.executionTypeList.append(["SimpleRicker", "betadiff"])
        self.executionTypeList.append(["SimpleWithMultiSelectivity", "betadiff"])
        self.executionTypeList.append(["ComplexTag", "betadiff"])
        self.executionTypeList.append(["SexedLengthBased", "betadiff"])
        self.executionTypeList.append(["BCO_tc", "betadiff"])
        self.executionTypeList.append(["HAK_tc", "betadiff"])
        self.executionTypeList.append(["HOK_tc", "betadiff"])
        self.executionTypeList.append(["LIN_tc", "betadiff"])
        self.executionTypeList.append(["ORH_tc", "betadiff"])
        self.executionTypeList.append(["SBW_tc", "betadiff"])
        self.executionTypeList.append(["SCI_tc", "betadiff"])
        self.executionTypeList.append(["LIN_tc_Dm", "betadiff"])
        self.executionTypeList.append(["SingleSexTagByLength_input", "betadiff"])
        self.executionTypeList.append(["SingleSexTagByLength_n", "betadiff"])
        self.executionTypeList.append(["TwoSex", "gammadiff"])
        self.executionTypeList.append(["TwoSexHybridMortality", "gammadiff"])
        self.executionTypeList.append(["SBW", "gammadiff"])
        self.executionTypeList.append(["Simple", "gammadiff"])
        self.executionTypeList.append(["SimpleRicker", "gammadiff"])
        self.executionTypeList.append(["SexedLengthBased", "gammadiff"])
        self.executionTypeList.append(["TwoSex", "adolc"])
        self.executionTypeList.append(["TwoSexHybridMortality", "adolc"])
        self.executionTypeList.append(["SBW", "adolc"])
        self.executionTypeList.append(["SBW_2022", "adolc"])
        self.executionTypeList.append(["Simple", "adolc"])
        self.executionTypeList.append(["SimpleRicker", "adolc"])
        self.executionTypeList.append(["SexedLengthBased", "adolc"])
        self.executionTypeList.append(["ORH3B", "simulate_dash_i"])
        self.executionTypeList.append(["SimAllObs", "simulate_dash_i"])
        self.executionTypeList.append(["Complex_input", "run_dash_i"])
        self.executionTypeList.append(["TwoSex_input", "run_dash_i"])
        self.executionTypeList.append(
            ["mcmc_start_mpd_mcmc_fixed", "resume_mcmc_from_mpd"]
        )
        self.executionTypeList.append(["mcmc_start_mpd", "resume_mcmc_from_mpd"])
        self.executionTypeList.append(["mcmc_resume", "resume_mcmc"])
        self.executionTypeList.append(["SingleSexTagByLength_input", "run_dash_I"])
        self.executionTypeList.append(["SingleSexTagByLength_n", "run_dash_I"])
        self.executionTypeList.append(["Simple", "projections"])
        self.executionTypeList.append(["SBW_2022", "tabular_tsv_mcmc"])
        # self.executionTypeList.append(["MultiSelectivity", "betadiff"])
        # self.executionTypeList.append(["SimpleExploitationRates", "projections"])
        # self.executionTypeList.append(["SimpleNoStdYcs", "adolc"])
        # self.executionTypeList.append(["SimpleNoStdYcs", "betadiff"])
        # self.executionTypeList.append(["SimpleNoStdYcs", "projections"])

        self.adolcLock = threading.Lock()

    def _print_log_tail(self, folder_path, execution_type, lines=100):
        """Print the last N lines of log/error files for a failed run."""
        log_files = []
        if execution_type == "betadiff":
            log_files = ["estimate_betadiff.log", "estimate_betadiff.err"]
        elif execution_type == "gammadiff":
            log_files = ["estimate_gammadiff.log", "estimate_gammadiff.err"]
        elif execution_type == "adolc":
            log_files = ["estimate_adolc.log", "estimate_adolc.err"]
        elif execution_type == "tabular_tsv_mcmc":
            log_files = [
                "mcmc_tsv_mpd.log",
                "mcmc_tsv_mpd.err",
                "mcmc_tsv.log",
                "mcmc_tsv.err",
            ]
        elif execution_type == "resume_mcmc_from_mpd":
            log_files = ["estimate.log", "esimate.err", "mcmc.log", "mcmc.err"]
        elif execution_type == "resume_mcmc":
            log_files = ["mcmc.log"]
        elif execution_type == "simulate_dash_i":
            log_files = ["multi_sim.log", "multi_sim.err"]
        elif execution_type in ["run_dash_i", "run_dash_I"]:
            log_files = ["run.log", "run.err"]
        elif execution_type == "projections":
            log_files = ["projections.log", "projections.err"]
        else:
            log_files = ["run.log", "run.err"]

        for log_file in log_files:
            file_path = os.path.join(folder_path, log_file)
            if os.path.exists(file_path):
                with open(file_path, "r", errors="replace") as f:
                    all_lines = f.readlines()
                tail = all_lines[-lines:]
                if tail:
                    print(f"  --- Last {len(tail)} lines of {log_file} ---")
                    for line in tail:
                        print(f"  {line}", end="")
                    print()

    def thread_target(
        self, folder, executionType, threadCwd, threadCommands, checkForFile=None
    ):
        self.result_container[f"{folder} - {executionType}"] = self.runCommandInThread(
            threadCwd, folder, executionType, threadCommands, checkForFile
        )

    """
    Utility functions to run from the threads
    """

    def _check_asserts_in_logs(self, folder_path, log_files):
        """Check logs for @assert success or failure."""
        for log_file in log_files:
            file_path = os.path.join(folder_path, log_file)
            if os.path.exists(file_path):
                with open(file_path, "r", errors="replace") as f:
                    content = f.read()
                if "Assert Failed" in content:
                    return False  # An assert failed
                if "Assert Passed" in content:
                    return True  # At least one assert passed
        return None  # No assert messages found

    def runCommandInThread(
        self, folderPath, folderName, executionType, commands, checkForFileList=[]
    ):
        for command in commands:
            if "-s" in command and not os.path.exists(folderPath + "/sim"):
                os.mkdir(folderPath + "/sim")

        requires_lock = executionType in ["betadiff"]
        if requires_lock:
            self.adolcLock.acquire()

        try:
            start = time.time()
            exit_code = EX_OK
            for command in commands:
                if exit_code != EX_OK:
                    break
                process = subprocess.Popen(
                    command,
                    cwd=folderPath,
                    stdout=subprocess.PIPE,
                    stderr=subprocess.PIPE,
                    shell=True,
                )
                output, error = process.communicate()
                exit_code = process.wait()
            elapsed = time.time() - start

            # Check logs for @assert results
            log_files = [
                "estimate_betadiff.log",
                "estimate_gammadiff.log",
                "estimate_adolc.log",
                "run.log",
            ]
            assert_check = self._check_asserts_in_logs(folderPath, log_files)
            if assert_check is False:
                exit_code = 1  # Mark as failed if any assert failed

        finally:
            if requires_lock:
                self.adolcLock.release()

        for command in commands:
            if "-s" in command and os.path.exists(folderPath + "/sim"):
                for checkForFile in checkForFileList:
                    if not os.path.exists(folderPath + "/sim/" + checkForFile):
                        exit_code = 1  # Indicate failure if expected file is not found
                for filename in os.listdir(folderPath + "/sim"):
                    file_path = os.path.join(folderPath + "/sim", filename)
                    os.remove(file_path)

        return (output, error, exit_code, elapsed, folderPath)

    """
    Start the modelrunner builder
    """

    def start(self):
        binary_name = "casal2"
        if Globals.operating_system_ == "windows":
            binary_name += ".exe"
        exe_path = f"Casal2/{binary_name}"

        if not os.path.exists(exe_path):
            print(f"Looking for {exe_path}")
            print(
                "The Casal2 binary (" + binary_name + ") was not found. Cannot continue"
            )
            print(
                "Please complete building the complete end-user application before running the models (i.e., doBuild archive"
            )
            return False

        print("")
        success_count = 0
        fail_count = 0

        dir_list = os.listdir("../TestModels/")
        cwd = os.path.normpath(os.getcwd())

        exe_path = f"{cwd}/{exe_path}"
        print(f"--> Full Casal2 path for model runner: {exe_path}")

        # Check that every model folder referenced in executionTypeList actually exists
        missing_folders = []
        for model_name, exec_type in self.executionTypeList:
            folder_path = "../TestModels/" + model_name
            if not os.path.isdir(folder_path):
                missing_folders.append(f"{model_name} (exec type '{exec_type}')")
        if missing_folders:
            print(
                "ERROR: The following model folders are listed in executionTypeList but do not exist:"
            )
            for entry in missing_folders:
                print(f"  - {entry}")
            return False

        threads = []

        # test -r functionality with full/different models
        print("Starting model runner with multiple threads...")
        print(f"Scanning a total of : {len(dir_list)-1} model folders")
        for folder in dir_list:
            if folder.startswith("."):
                continue
            if folder.startswith("DO NOT"):
                continue

            for model_name, exec_type in self.executionTypeList:
                if folder == model_name:
                    threadCommand = []
                    checkForFile = []
                    # This model has a mortality_hybrid time step with M but no fishery, which stops at Verify
                    verify_option = " -V warning" if model_name == "TwoSexHybridMortality" else ""

                    if exec_type == "betadiff":
                        threadCommand.append(
                            f"{exe_path} -e -g 20{verify_option} -c config_betadiff.csl2 > estimate_betadiff.log 2> estimate_betadiff.err"
                        )
                    if exec_type == "gammadiff":
                        threadCommand.append(
                            f"{exe_path} -e -g 20{verify_option} -c config_gammadiff.csl2 > estimate_gammadiff.log 2> estimate_gammadiff.err"
                        )
                    if exec_type == "adolc":
                        threadCommand.append(
                            f"{exe_path} -e -g 20{verify_option} --config config_adolc.csl2 > estimate_adolc.log 2> estimate_adolc.err"
                        )
                    if exec_type == "resume_mcmc_from_mpd":
                        threadCommand.append(
                            f"{exe_path} -E mpd.log > estimate.log 2> estimate.err"
                        )
                        threadCommand.append(
                            f"{exe_path} -M mpd.log > mcmc.log 2> mcmc.err "
                        )
                    if exec_type == "resume_mcmc":
                        threadCommand.append(
                            f"{exe_path} -R mpd.log --objective-file objectives.1 --sample-file samples.1 > mcmc.log 2>&1"
                        )
                    if exec_type == "simulate_dash_i":
                        threadCommand.append(
                            f"{exe_path} -s 10 -i samples.1  > multi_sim.log 2> multi_sim.err"
                        )
                        if folder == "ORH3B":
                            checkForFile.append("CPUEandes.1_01")
                            checkForFile.append("CPUEandes.9_10")
                            checkForFile.append("Obs_Andes_LF.3_05")
                        else:
                            checkForFile.append("sim_all_obs.1_01")
                            checkForFile.append("sim_all_obs.1_10")
                    if exec_type == "run_dash_i":
                        threadCommand.append(
                            f"{exe_path} -r -i pars.out > run.log 2> run.err"
                        )
                    if exec_type == "run_dash_I":
                        threadCommand.append(
                            f"{exe_path} -r -I pars.out > run.log 2> run.err"
                        )
                    if exec_type == "projections":
                        threadCommand.append(
                            f"{exe_path} -f 100 -i free.dat -g 20 -c config_projections.csl2 -t > projections.log 2> projections.err"
                        )
                    if exec_type == "tabular_tsv_mcmc":
                        threadCommand.append(
                            f"{exe_path} -E mpd_tsv.log -c config_mcmc_tsv.csl2 > mcmc_tsv_mpd.log 2> mcmc_tsv_mpd.err"
                        )
                        threadCommand.append(
                            f"{exe_path} -M mpd_tsv.log -T -c config_mcmc_tsv.csl2 > mcmc_tsv.log 2> mcmc_tsv.err"
                        )

                    if len(threadCommand) == 0:  # Default -r test
                        threadCommand.append(f"{exe_path} -r > run.log 2> run.err")

                    threadCwd = "../TestModels/" + folder
                    threads.append(
                        threading.Thread(
                            target=self.thread_target,
                            args=(
                                folder,
                                exec_type,
                                threadCwd,
                                threadCommand,
                                checkForFile,
                            ),
                        )
                    )
                    threads[-1].start()

        print(f"Total threads created: {len(threads)}")  # Add this line
        totalThreads = len(threads)
        while any(t.is_alive() for t in threads):
            alive_count = sum(1 for t in threads if t.is_alive())
            progressPercentage = ((totalThreads - alive_count) / totalThreads) * 100
            print(
                f"Progress: {progressPercentage:.2f}% - Waiting for {alive_count} models to complete..."
            )
            time.sleep(2)

        for i, thread in enumerate(threads):
            thread.join()

        for key, (
            output,
            error,
            exit_code,
            elapsed,
            folder_path,
        ) in self.result_container.items():
            if exit_code != EX_OK:
                print(
                    "[FAILED] - " + key + " in " + str(round(elapsed, 2)) + " seconds"
                )
                exec_type = key.split(" - ", 1)[1] if " - " in key else ""
                self._print_log_tail(folder_path, exec_type)
                fail_count += 1
            else:
                print("[OK] - " + key + " in " + str(round(elapsed, 2)) + " seconds")
                success_count += 1

        print("")
        print("Total Models: " + str(success_count + fail_count))
        print("Failed Models: " + str(fail_count))
        if fail_count > 0:
            print(
                "Please check the run.log or estimate.log file in each of the failed model directories"
            )
            return False
        return True


class UnitTests:
    def start(self):
        binary_name = "casal2"
        if Globals.operating_system_ == "windows":
            binary_name += ".exe"

        exe_path = (
            f"bin/{Globals.operating_system_}_{Globals.compiler_}/test/{binary_name}"
        )
        cwd = os.path.normpath(os.getcwd())
        exe_path = f"{cwd}/{exe_path}"

        print(exe_path)
        if not os.path.exists(exe_path):
            print(f"Looking for {exe_path}")
            print("CASAL2 binary was not found. Can not continue")
            print(
                "Please complete a release test binary build before running the models"
            )
            return False

        print("")
        start = time.time()

        if os.system(f"{exe_path} > unit_tests.log 2>&1") != EX_OK:
            # if os.system(f"{exe_path}") != EX_OK:
            elapsed = time.time() - start
            print("[FAILED] in " + str(round(elapsed, 2)) + " seconds")
        else:
            elapsed = time.time() - start
            print("[COMPLETED] in " + str(round(elapsed, 2)) + " seconds")

        os.chdir(cwd)
        return True
