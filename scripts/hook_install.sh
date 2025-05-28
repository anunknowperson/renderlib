#!/bin/bash

cd "$(git rev-parse --show-toplevel)" || {
  echo "Error: not inside a Git repository."
  exit 1
}

SOURCE_HOOK="scripts/pre-commit"
TARGET_HOOK=".git/hooks/pre-commit"

if [ ! -f "$SOURCE_HOOK" ]; then
  echo "Error: $SOURCE_HOOK not found."
  exit 1
fi

mkdir -p .git/hooks

cp "$SOURCE_HOOK" "$TARGET_HOOK" || {
  echo "Error: failed to copy $SOURCE_HOOK to $TARGET_HOOK"
  exit 1
}

chmod +x "$TARGET_HOOK"

echo "Installed pre-commit hook to $TARGET_HOOK"
