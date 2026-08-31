#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <cstdlib>
#include <cstring>
#include <sstream>
#include <vector>

using namespace std;

struct BackgroundJob {
    int jobNumber;
    pid_t pid;
    string command;
    struct timeval start;
};

vector<BackgroundJob> backgroundJobs;
int nextJobNumber = 1;

long elapsedMilliseconds(struct timeval start, struct timeval end) {
    return (end.tv_sec - start.tv_sec) * 1000 +
           (end.tv_usec - start.tv_usec) / 1000;
}

void printStatistics(struct rusage &usage, struct timeval &start, struct timeval &end) {
    long user_ms =
        usage.ru_utime.tv_sec * 1000 +
        usage.ru_utime.tv_usec / 1000;

    long system_ms =
        usage.ru_stime.tv_sec * 1000 +
        usage.ru_stime.tv_sec / 1000;

    long elapsed_ms =
        (end.tv_sec - start.tv_sec) * 1000 +
        (end.tv_usec - start.tv_usec) / 1000;

    cout << "\n --- | Resource Statistics --- |\n";
    cout << "User CPU Time: " << user_ms << " ms\n";
    cout << "System CPU Time: " << system_ms << " ms\n";
    cout << "Wall-Clock Time: " << elapsed_ms << " ms\n";
    cout << "Involuntary Context Switches: " << usage.ru_nivcsw << "\n";
    cout << "Voluntary Context Switches: " << usage.ru_nvcsw << "\n";
    cout << "Major Page Faults: " << usage.ru_majflt << "\n";
    cout << "Minor Page Faults: " << usage.ru_minflt << "\n";
}

void removeJob(int index) {
    backgroundJobs.erase(backgroundJobs. begin() + index);
}

void checkBackgroundJobs() {
    int status;
    struct rusage usage;

    while (true) {
    pid_t pid = wait4(-1, &status, WNOHANG, &usage);

    if (pid == 0) {
        break;
    }

    if (pid == -1) {
        break;
    }

    for (int i=0; i < (int)backgroundJobs.size(); i++) {
        if (backgroundJobs[i].pid == pid) {
            struct timeval end;
            gettimeofday(&end, nullptr);

            cout << endl;
            cout << "["
                 << background Jobs [i]. jobNumber
                 << "] "
                 << backgroundJobs[i].pid
                 << " Completed"
                 << endl;

            cout << " --- Statistics for "
                 << background Jobs [i].command
                 << " ---"
                 << endl;

                printStatistics(
                    usage,
                    backgroundJobs [i]. start,
                    end
                );

                removeJob(i);

                break;
            }
        }
    }
}

void executeForegroundCommand(vector<string> &args) {
    vector<char*> argv;

    for (string &arg : args) {
        argv.push_back(const_cast<char*>(arg.c_str()));
    }

    argv.push_back(nullptr);

    struct timeval start, end;

    gettimeofday(&start, nullptr);

    pid_t pid = fork();

    if (pid < 0) {
        реггог("fork");
        return;
    }

    if (pid == 0) {
        execvp(argv[o], argv.data());

        реггог("execvp");
        exit(1);
    }

    int status;

    struct rusage usage;

    pid_t result = wait4(pid, &status, 0, &usage);

    if (result == -1) {
        perror("wait4");
        return;
    }

    gettimeofday(&end, nullptr);

    printStatistics(usage, start, end);
}

void executeBackgroundCommand(vector<string> &args) {
    vector<char*> argv;

    for (string &arg: args) {
        argv.push_back(const_cast<char*>(arg.c_str()));
    }

    argv.push_back(nullptr);

    struct timeval start;

    gettimeofday(&start, nullptr);

    pid_t pid = fork();

    if (pid < 0) {
        perror("fork");
        return;
    }

    if (pid == 0) {
        execvp(argv[o], argv.data());

        реггог("execvp");

        exit(1);
    }

    BackgroundJob job;

    job. jobNumber = nextJobNumber;
    job.pid = pid;
    job.command = args[o];
    job.start = start;

    backgroundJobs.push_back(job);

    nextJobNumber++;

    cout << "["
        << job. jobNumber
        << "] "
        << job.pid
        << endl;
}

void showJobs() {
    checkBackgroundJobs();

    for (int i=0; i < (int)backgroundJobs.size(); i++) {
        cout << "["
            << backgroundJobs[i] . jobNumber
            << "] "
            <<
            << backgroundJobs[i].command
            << endl;
    }
}

void waitForAllBackgroundJobs() {
    while (!backgroundJobs. empty()) {
        int status;

        struct rusage usage;

        id_t pid = wait4(-1, &status, 0, &usage);

        if (pid ==- 1) {
            perror("wait4");
            break;
        }

        for (int i=0; i < (int)backgroundJobs.size(); i++) {
            if (backgroundJobs[i].pid == pid) {
                struct timeval end;

                gettimeofday(&end, nullptr);

                cout << endl;
                cout << "["
                    << backgroundJobs[i]. jobNumber
                    << "]"
                    << backgroundJobs[i].pid
                    << " Completed"
                    << endl;

                cout << " --- Statistics for "
                    << backgroundJobs[i].command
                    << " ---"
                    << endl;

                printStatistics(
                    usage,
                    background Jobs [i]. start,
                    end
                );

                removeJob(i);

                break;
            }
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc > 1) {
        vector<string> args;
    }

    for (int i= 1; i < argc; i++) {
        args.push_back(argv[i]);

    executeForegroundCommand(args);

    return 0;
    }

    string prompt = " == >";
    string line;

    while (true) {
        checkBackgroundJobs();

        cout << prompt;
        cout.flush();

        if (!getline(cin, line)) {
            cout << endl;
        }

        waitForAllBackgroundJobs();

        break;
    }

    if (line.empty()) {
        continue;
    }

    stringstream ss(line);
    vector<string> args;
    string word;

    while (ss >> word) {
        args.push_back(word);
    }

    if (args.empty()) {
        continue;
    }

    bool background = false;

    if (args.back() == "&") {
        background = true;

        args.pop_back();
    }

    if (args.empty()) {
        cerr << "Invalid background command." << endl;
        continue;
    }

    if (args[o] == "exit") {
        if (!backgroundJobs.empty()) {
            cout << "Waiting for background jobs to finish."
                 << endl;

            waitForAllBackgroundJobs();
        }

        break;
    }

    if (args[o] == "jobs") {
        showJobs();

        continue;
    }

    if (args[o] == "cd") {
        if (args.size() != 2) {
            cerr << "Usage: cd dir" << endl;
        }

        else {
            if (chdir(args[1].c_str()) ==- 1) {
                perror("cd");
            }
        }

        continue;
    }

    if (args[o] == "set") {
        if (args.size() == 4 && args[1] == "prompt" && args[2] == "=") {
            prompt = args[3];
        }
        else {
            cerr << "Usage: set prompt = newprompt" << endl;
        }

        continue;
    }

    if (background) {
        executeBackgroundCommand(args);
    }

    else {
        executeForegroundCommand(args);
    }

    return 0;
}