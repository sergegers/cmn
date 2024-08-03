."$PSScriptRoot/shared.ps1"

# You can write your powershell scripts inline here. 
# You can also pass predefined and custom variables to this scripts using arguments


<#	$environmentVars = Get-ChildItem -Path env:*

	ForEach($var In $environmentVars)
	{
		$keyname = $var.Key
		$keyvalue = $var.Value
    
		Write-Host "${keyname}: $keyvalue"
	}	

#>    

Function Build-B2Args()
{
    # build b2 arguments        
    $b2args = " --stagedir=$env:BUILD_SOURCESDIRECTORY"
    
    If (![string]::IsNullOrWhiteSpace($env:BOOST_ARCHITECTURE))
    { $b2args += " architecture=$env:BOOST_ARCHITECTURE" }

    If (![string]::IsNullOrWhiteSpace($env:BOOST_ADDRESS_MODEL))
    { $b2args += " address-model=$env:BOOST_ADDRESS_MODEL" }

    If (![string]::IsNullOrWhiteSpace($env:BOOST_CONFIGURATION))
    { $b2args += " variant=$env:BOOST_CONFIGURATION" }

    If (![string]::IsNullOrWhiteSpace($env:BOOST_LINK))
    { $b2args += " link=$env:BOOST_LINK runtime-link=$env:BOOST_LINK" }

    If (![string]::IsNullOrWhiteSpace($env:BOOST_THREADING))
    { $b2args += " threading=$env:BOOST_THREADING" }

    If (![string]::IsNullOrWhiteSpace($env:BOOST_INCLUDE))
    { $b2args += " include='$env:BOOST_INCLUDE'" }

    If (![string]::IsNullOrWhiteSpace($env:BOOST_LIB))
    { $b2args += " linkflags=/LIBPATH:'$env:BOOST_LIB'" }
    
    If (![System.Convert]::ToBoolean($env:BOOST_ALLOW_BUILD_ERRORS))
    { $b2args += " -q" }

    If (![string]::IsNullOrWhiteSpace($env:BOOST_SKIP_LIBS))
    { $b2args += $env:BOOST_SKIP_LIBS.Split(',') | % { " --without-$_" } }

    $b2args += " stage $env:BOOST_ADDITIONAL_B2_ARGS"

    #build cpp flags
    $cxx_flags = ""
    
    If (![string]::IsNullOrWhiteSpace($env:CPP_LANGUAGE))
    { $cxx_flags += "/std:$env:CPP_LANGUAGE " }

    If (![System.Convert]::ToBoolean($env:CPP_ELIMINATE_OLD_STL))
    { $cxx_flags += "/D'_HAS_AUTO_PTR_ETC=1' " }

    If ([System.Convert]::ToBoolean($env:BOOST_DISABLE_RANGE_IO))
    { $cxx_flags += "/DCHT_DISABLE_RANGE_IO " }

    #TODO: process CPP_DISABLE_RTTI

    If (![string]::IsNullOrWhiteSpace($env:BOOST_CXX_FLAGS))
    { $cxx_flags += " $env:BOOST_CXX_FLAGS" }

    If (![string]::IsNullOrWhiteSpace($cxx_flags))
    { $b2args += " --cxxflags='$cxx_flags'" }

    Return $b2args.Replace("'", "`"")
}
#####################################################################################################
$ErrorActionPreference = "Stop"

# check prerequisites        
If ((Shrink-Str $env:BOOST_VERSION) -eq "")
{
    Write-Error @"  
Please, set the boost.version variable!
It must have format "boost_x_yz_t[_b]" where
x - major version
yz - minor version
t - patch level
b (optional) - beta version
"@ 
    Exit 1
}
    
# set internal build variables

#get execution options
[bool] $has_b2 = [System.IO.File]::Exists("$env:BUILD_SOURCESDIRECTORY\b2.exe")
[bool] $clean = [System.Convert]::ToBoolean($env:BUILD_REPOSITORY_CLEAN)             

# can be missed
[string] $sync_sources_ = $env:BUILD_SYNC_SOURCES
[bool] $sync_sources;
If ($sync_sources_ -eq "") { $sync_sources = $True }
Else { $sync_sources = [System.Convert]::ToBoolean($sync_sources_) }

[bool] $allow_build_errors = [System.Convert]::ToBoolean($env:BOOST_ALLOW_BUILD_ERRORS)
[bool] $debug = [System.Convert]::ToBoolean($env:SYSTEM_DEBUG)
[bool] $skip_patches = [System.Convert]::ToBoolean($env:BUILD_SKIP_PATCHES)

# build b2 arguments
[string] $b2args = Build-B2Args

# propagate execution options
# NOTE: keep boolean values as strings
Set-Var "build.has_b2" $([System.Convert]::ToString($has_b2))
Set-Var "boost.b2_args" $b2args

Write-Host "Checking options consistency..."

# check b2.exe
If (!$clean -and !$has_b2)
{
    Write-Error "b2.exe isn't found. Restart in clean mode"            
    Exit 1
}

If ($clean)
{
    If (!$sync_sources)
    {
        Write-Error @"
Source synchronization is not set within the clean mode.
This option combination is forbidden. Enable
synchronization and restart build.
"@
        Exit 1
    }    
}

If ($debug)
{
    If (!$clean)
    {
        Write-Error @"
Clean mode required in accordance with the debug mode.
Enable clean mode or disable debug one.
"@
        Exit 1
    }
}

Write-Host "Done."

Write-Host @"
Execution options: clean [$clean], allow build errors [$allow_build_errors], 
debug [$debug], skip applying patches [$skip_patches]
"@

Write-Host "b2 arguments are: [$b2args]"

# switch back to the master branch    
Invoke-Git "checkout --force $env:BUILD_SOURCEBRANCHNAME"

If ($debug)
{
    # to propagate the latest build script (this file) changes
    # force update the agent working directory
    git reset --hard origin/$env:BUILD_SOURCEBRANCHNAME 
    git status
    git diff origin/master        
}

If ($clean)
{
    git config user.name "Mr. Build Agent"
    git config user.email "menusman@gmail.com"
}

If (($clean -or $debug) -and !$skip_patches)
{
    # apply patches
    Write-Host "Applying patches..."

    $patches = Get-ChildItem -Path "$env:BUILD_SOURCESDIRECTORY\_build\patch\*.patch"
    ForEach ($patch in $patches)
    {
        Invoke-Git "apply `"$patch`" --index --ignore-space-change --whitespace=nowarn --verbose" 
    }
        
    Write-Host "Done."        
}
Else
{
    Write-Warning @"
Patches aren't applied. DO NOT DO THIS ON FRESHLY LOADED SOURCES. 
To enable applying patches set build.repository.clean variable to [true]
or system.debug one to [true] and set set build.skip_patches to [false] 
(used for debug purposes).
"@        
}
