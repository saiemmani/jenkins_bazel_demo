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
                sh 'git config --global user.name "jean-khoury"'
                sh 'git config --global user.email "jeankhoury_98@hotmail.com"'
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