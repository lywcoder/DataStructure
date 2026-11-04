[CmdletBinding()]
param(
    [switch]$Quiet
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

$repositoryRoot = Split-Path -Parent $PSScriptRoot
$temporaryRoot = Join-Path ([System.IO.Path]::GetTempPath()) (
    'datastructure-verify-' + [System.Guid]::NewGuid().ToString('N'))
New-Item -ItemType Directory -Path $temporaryRoot | Out-Null

try {
    Get-Command g++ -ErrorAction Stop | Out-Null
    $entries = Get-ChildItem -LiteralPath $repositoryRoot -Directory |
        Where-Object { $_.Name -match '^Chapter\d+$' } |
        ForEach-Object { Get-ChildItem -LiteralPath $_.FullName -Recurse -Filter '*.cpp' } |
        Where-Object { Select-String -LiteralPath $_.FullName -Pattern '\bint\s+main\s*\(' -Quiet } |
        Sort-Object FullName

    $passed = 0
    foreach ($entry in $entries) {
        $relative = $entry.FullName.Substring($repositoryRoot.Length + 1).Replace('\', '/')
        $safeName = $relative.Replace('/', '_').Replace('.cpp', '.exe')
        $executable = Join-Path $temporaryRoot $safeName
        $sources = @($entry.FullName)

        if ($relative -eq 'Chapter1/currency/main.cpp' -or
            $relative -eq 'Chapter1/testCurrency.cpp') {
            $sources += Join-Path $repositoryRoot 'Chapter1/currency/currency.cpp'
        }
        if ($relative -eq 'Chapter6/chapter6Demo.cpp' -or
            $relative -eq 'Chapter6/testChapter6.cpp') {
            $sources += Join-Path $repositoryRoot 'Chapter6/binSort.cpp'
        }

        if (-not $Quiet) { Write-Host ('[build] ' + $relative) }
        $arguments = @('-std=c++11', '-O2', '-Wall', '-Wextra', '-Wpedantic', '-Werror')
        $arguments += $sources
        $arguments += @('-o', $executable)
        & g++ @arguments
        if ($LASTEXITCODE -ne 0) {
            throw "Compilation failed: $relative"
        }

        if (-not $Quiet) { Write-Host ('[run]   ' + $relative) }
        if ($Quiet) {
            $programOutput = & $executable 2>&1
        } else {
            & $executable
        }
        if ($LASTEXITCODE -ne 0) {
            if ($Quiet) { $programOutput | ForEach-Object { Write-Host $_ } }
            throw "Program failed with exit code $LASTEXITCODE`: $relative"
        }
        ++$passed
    }

    $headers = Get-ChildItem -LiteralPath $repositoryRoot -Directory |
        Where-Object { $_.Name -match '^Chapter\d+$' } |
        ForEach-Object { Get-ChildItem -LiteralPath $_.FullName -Recurse -File } |
        Where-Object { $_.Extension -eq '.hpp' -or $_.Extension -eq '.h' } |
        Sort-Object FullName
    $headerCount = 0
    foreach ($header in $headers) {
        $relative = $header.FullName.Substring($repositoryRoot.Length + 1).Replace('\', '/')
        if (-not $Quiet) { Write-Host ('[header] ' + $relative) }
        $source = "#include `"$relative`"`nint main() { return 0; }`n"
        $compilerOutput = $source | & g++ -x c++ -std=c++11 -O2 -Wall -Wextra `
            -Wpedantic -Werror -I $repositoryRoot -fsyntax-only - 2>&1
        if ($LASTEXITCODE -ne 0) {
            $compilerOutput | ForEach-Object { Write-Host $_ }
            throw "Standalone header compilation failed: $relative"
        }
        ++$headerCount
    }

    Write-Host ("Verified {0} programs and {1} standalone headers successfully." -f `
        $passed, $headerCount)
}
finally {
    if (Test-Path -LiteralPath $temporaryRoot) {
        $resolvedTemp = [System.IO.Path]::GetFullPath($temporaryRoot)
        $systemTemp = [System.IO.Path]::GetFullPath([System.IO.Path]::GetTempPath())
        if ($resolvedTemp.StartsWith($systemTemp, [System.StringComparison]::OrdinalIgnoreCase) -and
            (Split-Path -Leaf $resolvedTemp) -like 'datastructure-verify-*') {
            Remove-Item -LiteralPath $resolvedTemp -Recurse -Force
        }
    }
}
