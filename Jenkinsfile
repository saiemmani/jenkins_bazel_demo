pipeline {
    agent any

    stages {
        stage('Build') {
            steps {
                sh 'bazel build ...'
                publishChecks name: 'example', title: 'Pipeline Check', summary: 'check through pipeline'
            }
        }
              stage('Test') {
            steps {
                sh 'echo ehfweu'
            }
        }

                      stage('Deploy') {
            steps {
                sh 'echo deploying'
            }
        }
    }
}