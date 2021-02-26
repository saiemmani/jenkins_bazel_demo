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
        stage('Generate_documentation'){
            steps{
                sh 'python3 generate_documentation.py -r ./main'
                sh 'echo $(pwd)'
            }
        }
        stage('Push'){
            steps{
                sh 'git add ./linting'
                sh 'git add ./main'
                sh 'git commit -am "added from jenkins"'
                sh "git push origin master"
            }
        }
        stage('Deploy') {
            steps {
                echo 'Deploying....'
            }
        }
    }
}