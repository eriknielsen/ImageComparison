param(
    [Parameter(Mandatory=$true)] [string]$InputFolder,
    [Parameter(Mandatory=$true)] [string]$OutputFolder,
    [Parameter(Mandatory=$true)] [float]$MinimumSimilarity
)

# Get absolute path to comparer and output folder
$pathToImageComparer = Join-Path (Split-Path -Parent $MyInvocation.MyCommand.Path) "ImageComparison.exe"
# Make sure output folder exists
New-Item -ItemType Directory -Path $OutputFolder -Force | Out-Null
$pathToOutput = Resolve-Path $OutputFolder

# Clear out the output folder
Remove-Item -Recurse -Force $OutputFolder 

# For each folder in the input, assume there's 2 images to compare 
$inputFolders = Get-ChildItem -Path $InputFolder -Directory
foreach ($folder in $inputFolders) {
    Push-Location $folder.FullName
    try {
        $similarity = & $pathToImageComparer a.png b.png "results"
        Write-Host "Processing set: $($folder.FullName). Similarity factor of $similarity"
        # If input is too different (graphical artifacts have been introduced)
        # copy the input to the output for further processing
        if ($similarity -lt $MinimumSimilarity) {
            Write-Host "$($folder.FullName) is too different"
            Copy-Item -Recurse "results\\" (Join-Path $pathToOutput $folder.Name)
        }

    }
    finally {
        Pop-Location
    }
}