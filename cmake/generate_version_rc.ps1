param(
    [Parameter(Mandatory=$true)][string]$VersionFile,
    [Parameter(Mandatory=$true)][string]$TemplateFile,
    [Parameter(Mandatory=$true)][string]$OutputFile,
    [Parameter(Mandatory=$false)][string]$InternalName = "",
    [Parameter(Mandatory=$false)][string]$OriginalFilename = "",
    [Parameter(Mandatory=$false)][string]$FileDescription = ""
)

try {
    $v = Get-Content -Raw -Path $VersionFile -ErrorAction Stop
    $v = $v.Trim()
    if (-not $v) { throw "Empty version file" }

    $parts = $v.Split('.')
    $major = $parts[0]
    $minor = if ($parts.Length -ge 2) { $parts[1] } else { 0 }
    $patch = if ($parts.Length -ge 3) { $parts[2] } else { 0 }

    $template = Get-Content -Raw -Path $TemplateFile -ErrorAction Stop
    $out = $template -replace '@PROJECT_VERSION_MAJOR@', $major -replace '@PROJECT_VERSION_MINOR@', $minor -replace '@PROJECT_VERSION_PATCH@', $patch -replace '@PROJECT_VERSION@', $v
    $out = $out -replace '@RC_INTERNAL_NAME@', [System.Text.RegularExpressions.Regex]::Escape($InternalName)
    $out = $out -replace '@RC_ORIGINAL_FILENAME@', [System.Text.RegularExpressions.Regex]::Escape($OriginalFilename)
    $out = $out -replace '@RC_FILE_DESCRIPTION@', [System.Text.RegularExpressions.Regex]::Escape($FileDescription)

    $dir = Split-Path -Parent $OutputFile
    if (-not (Test-Path $dir)) { New-Item -ItemType Directory -Path $dir | Out-Null }

    Set-Content -Path $OutputFile -Value $out -Encoding UTF8
    exit 0
}
catch {
    Write-Error $_
    exit 1
}