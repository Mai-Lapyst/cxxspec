pipeline {
    agent { label 'bullseye' }
    stages {
        stage('System-prepare') {
            steps {
                sh 'apt update'
                sh 'apt install -y build-essential'
            }
        }
        stage('Building libcxxspec') {
            steps {
                checkout scm
                sh 'CC=g++ CXX=g++ make libcxxspec'
                sh 'PREFIX=./dist make install'
                sh 'cd ./dist && tar -cvf ../cxxspec_dist.tar.gz *'
                archiveArtifacts artifacts: 'cxxspec_dist.tar.gz', followSymlinks: false
            }
        }
        stage('Building & Running specs') {
            steps {
                sh 'make specs && cd ./build && ./specs.run --format junit -f specs-report.xml'
                junit skipPublishingChecks: true, testResults: 'build/specs-report.xml'
            }
        }
    }
}