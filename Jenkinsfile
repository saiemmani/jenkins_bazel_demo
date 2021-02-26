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
                sh 'git add ./linting'
                sh 'git commit -m "Added Linting" '
            }
        }
        stage('Push'){
            steps{
                hub config --global hub.protocol ssh
                hub config --global user.email "jeankhoury_98@hotmail.com"
                hub config --global user.name "jean-khoury"
                sh 'git push --set-upstream origin master'
            }
        }
        stage('Deploy') {
            steps {
                echo 'Deploying....'
            }
        }
    }
}