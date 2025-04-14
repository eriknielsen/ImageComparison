param(
    [Parameter(Mandatory=$true)] [string]$InputFolder,
    [Parameter(Mandatory=$true)] [string]$OutputFolder,
    [Parameter(Mandatory=$true)] [float]$MinimumSimilarity
)

# Get absolute path to comparer
$pathToImageComparer = Join-Path (Split-Path -Parent $MyInvocation.MyCommand.Path) "ImageComparison.exe"

# Clear out the output folder
Remove-Item -Recurse -Force $OutputFolder 

$inputFolders = Get-ChildItem -Path $InputFolder -Directory
foreach ($folder in $inputFolders) {
    Write-Host "Processing folder: $($folder.FullName)"

    Push-Location $folder.FullName
    try {
        $similarity = & $pathToImageComparer a.png b.png "results"

        # If input is too different (graphical artifacts have been introduced)
        # copy the input to the output for further processing
        if ($similarity -lt $MinimumSimilarity) {
            Copy-Item -Recurse "results\\" (Join-Path $OutputFolder $folder.Name)
        }

    }
    finally {
        Pop-Location
    }
}