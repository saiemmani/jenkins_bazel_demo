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
                sh 'bazel test //test:test'
            }
        }
        stage('Linting'){
            steps{
                sh 'python3 format-code.py -r ./main'
                sh 'echo $(pwd)'
            }
        }
        stage('Push'){
            steps{
                sh 'git add ./linting'
                sh 'git commit -am "added linting"'
                sh "git push origin master"
            }
        }
        stage('Generate_documentation'){
            steps{
                sh 'python3 format-code.py -r ./main'
            }
        }
        stage('Deploy') {
            steps {
                echo 'Deploying....'
            }
        }
    }
}