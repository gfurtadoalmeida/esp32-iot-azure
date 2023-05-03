function Rename-Project {
    param (
        [string]
        $SourceRootPath,
        [string]
        $ComponentName
    )

    $ComponentName = $ComponentName.Replace('-', '_').ToLower()
    $ComponentNameUpper = $ComponentName.ToUpper();

    $TextReplacements = New-Object System.Collections.ArrayList
    $TextReplacements.AddRange((
            [Tuple]::Create("$($SourceRootPath)/components/component_name/include/component_name/component_name.h", 'COMPONENT_NAME', $ComponentNameUpper),
            [Tuple]::Create("$($SourceRootPath)/components/component_name/include/component_name/component_name.h", 'component_name', $ComponentName),
            [Tuple]::Create("$($SourceRootPath)/components/component_name/private_include/assertion.h", 'COMPONENT_NAME', $ComponentNameUpper),
            [Tuple]::Create("$($SourceRootPath)/components/component_name/private_include/config.h", 'COMPONENT_NAME', $ComponentNameUpper),
            [Tuple]::Create("$($SourceRootPath)/components/component_name/private_include/log.h", 'COMPONENT_NAME', $ComponentNameUpper),
            [Tuple]::Create("$($SourceRootPath)/components/component_name/src/component_name.c", 'COMPONENT_NAME', $ComponentNameUpper),
            [Tuple]::Create("$($SourceRootPath)/components/component_name/src/component_name.c", 'component_name', $ComponentName),
            [Tuple]::Create("$($SourceRootPath)/components/component_name/test/CMakeLists.txt", 'component_name', $ComponentName),
            [Tuple]::Create("$($SourceRootPath)/components/component_name/Kconfig", 'Component_Name', $ComponentName),
            [Tuple]::Create("$($SourceRootPath)/components/component_name/Kconfig", 'COMPONENT_NAME', $ComponentNameUpper),
            [Tuple]::Create("$($SourceRootPath)/main/CMakeLists.txt", 'component_name', $ComponentName),
            [Tuple]::Create("$($SourceRootPath)/main/main.c", 'component_name', $ComponentName),
            [Tuple]::Create("$($SourceRootPath)/test/CMakeLists.txt", 'component_name', $ComponentName),
            [Tuple]::Create("$($SourceRootPath)/CMakeLists.txt", 'component_name', $ComponentName),
            [Tuple]::Create("$($SourceRootPath)/sonar-project.properties", 'component_name', $ComponentName)
        ))

    $FileRenamings = New-Object System.Collections.ArrayList
    $FileRenamings.AddRange((
            [Tuple]::Create("$($SourceRootPath)/components/component_name/include/component_name/component_name.h", "$($ComponentName).h"),
            [Tuple]::Create("$($SourceRootPath)/components/component_name/src/component_name.c", "$($ComponentName).c")
        ))

    $DirectoryRenamings = New-Object System.Collections.ArrayList
    $DirectoryRenamings.AddRange((
            [Tuple]::Create("$($SourceRootPath)/components/component_name/include/component_name", $ComponentName),
            [Tuple]::Create("$($SourceRootPath)/components/component_name", $ComponentName)
        ))

    foreach ($TextReplacement in $TextReplacements) {
        ((Get-Content -Path $TextReplacement.Item1 -Raw) -creplace $TextReplacement.Item2, $TextReplacement.Item3) | Set-Content -Path $TextReplacement.Item1
    }

    foreach ($FileRenaming in $FileRenamings) {
        Rename-Item $FileRenaming.Item1 $FileRenaming.Item2
    }

    foreach ($DirectoryRenaming in $DirectoryRenamings) {
        Rename-Item $DirectoryRenaming.Item1 $DirectoryRenaming.Item2
    }
}

Rename-Project -SourceRootPath $args[0] -ComponentName $args[1]