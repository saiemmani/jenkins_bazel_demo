pipeline {
    agent any

    stages {
        stage('Build') {
            steps {
                sh 'bazel build ...'
                // publishChecks name: 'BuildCheck', title: 'Build Check', summary: 'Calling Bazel build on the repo'
            }
        }
              stage('Test') {
            steps {
                sh 'echo ehfweu'
                // publishChecks name: 'TestCheck', title: 'Test Check', summary: 'Calling Bazel test on the repo'
            }
        }

            stage('Deploy') {
            steps {
                sh 'echo deploying'
                // publishChecks name : 'DeploymentCheck' , title: 'Deployment Check', summary :'Deploying the build artifacts'
            }
            
        }
    }
}