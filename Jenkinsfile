pipeline {
agent {
    // Equivalent to "docker build -f Dockerfile.build --build-arg version=1.0.2 ./build/
    dockerfile {
        dir 'docker'
        
    }
    stages {
        stage('Test') {
            steps {
                sh 'gcc --version'
            }
        }
    }
}
}