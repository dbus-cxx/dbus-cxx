pipeline {
	agent { label 'master' }

	stages {
		stage('Cleanup'){
			steps{
				cleanWs()
			}
		}

		stage('Checkout'){
			steps{
				fileOperations([folderCreateOperation('source')])
				dir('source'){
					checkout scm
				}
			}
		}

		stage('Build'){
			steps{
				buildDebPkg( "amd64", "buster" )
			}
		}
	}
}

void buildDebPkg(String arch, String distro){
	debianPbuilder additionalBuildResults: '', 
			architecture: arch, 
			components: '', 
			distribution: distro, 
			keyring: '', 
			mirrorSite: 'https://deb.debian.net/debian', 
			pristineTarName: ''
}
