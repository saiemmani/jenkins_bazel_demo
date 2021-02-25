pipeline {
    agent any

    stages {
        stage('Build') {
            steps {
                sh 'bazel build //main:main'
            }
        }
        stage('Test') {
            steps {
                echo 'Testing..'
            }
        }
        stage('Deploy') {
            steps {
                echo 'Deploying....'
            }
        }
    }
}