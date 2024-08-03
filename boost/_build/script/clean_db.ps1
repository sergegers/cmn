."$PSScriptRoot/shared.ps1"

#clean old binaries
Write-Host "Drop old database binaries and compress database."

# https://www.link-intersystems.com/blog/2014/07/17/remove-directories-and-files-permanently-from-git/

Write-Host "Clearing binary files from index..."

$env:FILTER_BRANCH_SQUELCH_WARNING = 1
Invoke-Git "filter-branch -f --index-filter 'git rm --force --cached --ignore-unmatch *.exe *.lib *.dll' --prune-empty --tag-name-filter cat -- --all"

Write-Host "Deleting backup..."       
git for-each-ref --format="%(refname)" refs/original/ | % { git update-ref -d $_ }

Write-Host "Collecting garbage..."
git gc --prune=now

Write-Host "Done."        
