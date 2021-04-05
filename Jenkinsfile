pipeline {
    agent any

    stages {
        stage('Build') {
            steps {
                sh 'bazel build ...'
                sh 'sleep 7'
                publishChecks name: 'BuildCheck', title: 'Build Check', summary: 'Calling Bazel build on the repo'
            }
        }
            stage('Test') {
            steps {
                script{
                    sh 'sleep 7'
                    sh 'echo ehfweu'
                    publishChecks name: 'TestCheck', title: 'Test Check', summary: 'Calling Bazel testt on the repo'
                }
            }
        }

            stage('Deploy') {
            steps {
                script{
                try{
                echo 'testDeploy'
                sh 'sleep 7'
                sh 'echo deploying'
                publishChecks name : 'DeployCheck' , title: 'Deployment Check', summary :'Deploying the build artifacts' 
                }
                catch (exp){
                    publishChecks name : 'Deploy Check Failure' , title: 'Deployment Check Failure', summary :'Deploying the build artifacts', conclusion : 'FAILURE' , text : exp.message
                    sh 'exit -1'
                }
                }
            }
            
        }
    }
}