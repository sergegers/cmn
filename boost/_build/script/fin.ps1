."$PSScriptRoot/shared.ps1"

#load compiled binaries 
Write-Host "Loading compiled BOOST static and dynamic libraries..." 

Write-Host "Adding..."
git add  --verbose "$env:BUILD_SOURCESDIRECTORY/lib/*"
    
Write-Host "Committing binaries for [$env:BOOST_VERSION] ..."
git commit --all --verbose --message "binaries for $env:BOOST_VERSION"

Write-Host "Pushing..."
# push force to avoid merge conflicts
Invoke-Git "push --force --verbose"

Write-Host "Library [$env:BOOST_VERSION] build completed." 
