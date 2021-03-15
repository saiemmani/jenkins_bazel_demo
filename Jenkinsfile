pipeline {
    agent any

    stages {
        stage('Build') {
            steps {
                sh 'bazel build ...'
                publishChecks name: 'pipeline check', title: 'pipeline ', summary: '# A pipeline check example'
            }
        }  
    }
}