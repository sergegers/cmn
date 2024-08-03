Function Shrink-Str([string] $str)
{
    $str = $str.Replace(" ", "")
    Return $str
}

<#
    https://stackoverflow.com/a/45224858/8452129

    .Synopsis
        Invoke git, handling its quirky stderr that isn't error

    .Outputs
        Git messages, and lastly the exit code

    .Example
        Invoke-Git push

    .Example
        Invoke-Git "add ."
#>
Function Invoke-Git([string] $cmd)
{
    $backup_ea = $ErrorActionPreference
    $ErrorActionPreference = "Continue"
    
    Try 
    {
        Write-Host "git $cmd"
        $err = Invoke-Expression "git $cmd 2>&1"             
        If ($LASTEXITCODE -gt 0 )
        {            
            $err | Select-Object -Skip 1 | Out-Host
        }
        Else
        {
            Write-Host $err
        }
        
    }
    Catch
    {
        $LASTEXITCODE = 1
        Write-Error "#git exception: $_`n$($_.ScriptStackTrace)"
    }
    Finally
    {
        $ErrorActionPreference = $backup_ea
    }
}

Function Set-Var([string] $name, $value)
{
    # https://roadtoalm.com/2016/08/11/set-output-variable-in-a-powershell-vsts-build-task/
    Write-Output ("##vso[task.setvariable variable=$name;]$value")
}

