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
				configFileProvider([configFile(fileId: 'f5846b83-6547-46c8-9b33-1dbc94441a3e', targetLocation: 'hookdir/D21-nightly-stretch')]) {
					buildDebPkg( "amd64", "buster" )
				}
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
